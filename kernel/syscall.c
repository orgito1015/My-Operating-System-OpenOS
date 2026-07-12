/*
 * OpenOS - System Call Implementation (Phase 1)
 *
 * Dispatches int 0x80 requests. The most involved call is fork():
 *
 *   1. Allocate a child PCB, kernel stack, and user stack.
 *   2. Copy the parent's entire user stack into the child's and shift
 *      every saved-EBP link in the copied frame-pointer chain by the
 *      distance between the two stacks, so the child's frames reference
 *      its own stack instead of the parent's.
 *   3. Build the child's kernel stack: a copy of the parent's syscall
 *      register frame with EAX = 0 (fork returns 0 in the child) and
 *      ESP/EBP relocated, sitting above a context_switch frame whose
 *      return target is fork_child_return. When the scheduler first
 *      picks the child, context_switch "returns" into that stub, which
 *      unwinds the register frame and IRETs straight back to the
 *      instruction after the parent's `int $0x80` — in the child's own
 *      address context.
 */

#include "syscall.h"
#include "kernel.h"
#include "../process/process.h"
#include "../process/scheduler.h"
#include "../memory/heap.h"
#include "../drivers/console.h"
#include "../arch/x86/idt.h"
#include "../arch/x86/gdt.h"

/* Assembly stubs */
extern void int80_handler(void);      /* arch/x86/syscall.S  */
extern void fork_child_return(void);  /* arch/x86/context.S  */

void syscall_init(void) {
    /*
     * DPL 3 interrupt gate: user code may execute int 0x80, and the
     * gate type clears IF on entry so the frame is captured atomically.
     */
    idt_set_gate(0x80, (uint32_t)int80_handler,
                 KERNEL_CODE_SEGMENT, IDT_FLAGS_USER);
    console_write("Syscalls: int 0x80 gate installed (8 syscalls)\n");
}

/* ------------------------------------------------------------------ */
/* sys_write                                                            */
/* ------------------------------------------------------------------ */

static int sys_write(const char *str) {
    if (!str) return -1;
    int n = 0;
    /* Bounded to keep a bad pointer from wedging the console forever. */
    while (str[n] && n < 4096) {
        console_put_char(str[n]);
        n++;
    }
    return n;
}

/* ------------------------------------------------------------------ */
/* sys_fork                                                             */
/* ------------------------------------------------------------------ */

static int sys_fork(regs_t *parent_regs) {
    process_t *parent = process_current();

    if (!parent->is_user || !parent->ustack) {
        return -1;   /* fork() is only defined for user processes */
    }

    uint8_t *child_kstack = (uint8_t *)kmalloc(PROCESS_KSTACK_SIZE);
    if (!child_kstack) return -1;

    uint8_t *child_ustack = (uint8_t *)kmalloc(PROCESS_USTACK_SIZE);
    if (!child_ustack) {
        kfree(child_kstack);
        return -1;
    }

    __asm__ __volatile__("cli");

    /* PCB */
    process_t *child = 0;
    for (int i = 0; i < PROCESS_MAX; i++) {
        process_t *slot = process_table_entry(i);
        if (slot && slot->state == PROCESS_STATE_UNUSED) {
            child = slot;
            break;
        }
    }
    if (!child) {
        __asm__ __volatile__("sti");
        kfree(child_kstack);
        kfree(child_ustack);
        return -1;
    }

    /* Start from a copy of the parent's PCB, then fix up identity. */
    *child = *parent;
    child->next           = 0;
    child->parent_waiting = 0;
    child->cpu_ticks      = 0;
    child->wait_ticks     = 0;

    /* New pid: reuse process_by_pid-safe allocation via a scan. */
    {
        uint32_t pid = 1;
        for (;;) {
            int taken = 0;
            for (int i = 0; i < PROCESS_MAX; i++) {
                process_t *s = process_table_entry(i);
                if (s && s != child && s->state != PROCESS_STATE_UNUSED &&
                    s->pid == pid) {
                    taken = 1;
                    break;
                }
            }
            if (!taken) break;
            pid++;
        }
        child->pid = pid;
    }
    child->ppid = parent->pid;

    /* Stacks */
    child->kstack     = child_kstack;
    child->kstack_top = (uint32_t)child_kstack + PROCESS_KSTACK_SIZE;
    child->ustack     = child_ustack;
    child->ustack_top = (uint32_t)child_ustack + PROCESS_USTACK_SIZE
                        - ((uint32_t)parent->ustack + PROCESS_USTACK_SIZE
                           - parent->ustack_top);

    /* 2. Copy the user stack and relocate the frame-pointer chain. */
    uint32_t parent_lo = (uint32_t)parent->ustack;
    uint32_t parent_hi = parent_lo + PROCESS_USTACK_SIZE;
    int32_t  delta     = (int32_t)((uint32_t)child_ustack - parent_lo);

    for (uint32_t i = 0; i < PROCESS_USTACK_SIZE; i++) {
        child_ustack[i] = parent->ustack[i];
    }

    /* Walk the saved-EBP chain in the child's copy, shifting each link. */
    {
        uint32_t ebp = parent_regs->ebp;
        int guard = 64;
        while (guard-- > 0 && ebp >= parent_lo &&
               ebp <= parent_hi - sizeof(uint32_t) && (ebp & 3) == 0) {
            uint32_t *child_slot = (uint32_t *)(ebp + delta);
            uint32_t next_ebp = *child_slot;   /* same value as parent's */
            if (next_ebp >= parent_lo && next_ebp <= parent_hi) {
                *child_slot = next_ebp + delta;
            }
            if (next_ebp <= ebp) break;        /* chain must ascend      */
            ebp = next_ebp;
        }
    }

    /* 3. Build the child's kernel stack. */
    uint32_t sp = child->kstack_top;

    /* 3a. Copied register frame (top of kernel stack). */
    sp -= sizeof(regs_t);
    regs_t *child_regs = (regs_t *)sp;
    *child_regs = *parent_regs;
    child_regs->eax = 0;                            /* fork() -> 0      */
    child_regs->useresp = parent_regs->useresp + delta;
    if (parent_regs->ebp >= parent_lo && parent_regs->ebp <= parent_hi) {
        child_regs->ebp = parent_regs->ebp + delta;
    }

    /* 3b. context_switch frame beneath it. */
    uint32_t *csp = (uint32_t *)sp;
    *(--csp) = (uint32_t)fork_child_return;  /* ret target              */
    *(--csp) = 0x002;                        /* EFLAGS: IF=0 until IRET */
    *(--csp) = 0;                            /* EBP                     */
    *(--csp) = 0;                            /* EBX                     */
    *(--csp) = 0;                            /* ESI                     */
    *(--csp) = 0;                            /* EDI                     */
    child->esp = (uint32_t)csp;

    child->state = PROCESS_STATE_READY;
    scheduler_enqueue(child);

    __asm__ __volatile__("sti");
    return (int)child->pid;
}

/* ------------------------------------------------------------------ */
/* Dispatcher                                                           */
/* ------------------------------------------------------------------ */

void syscall_dispatch(regs_t *r) {
    switch (r->eax) {
        case SYS_EXIT:
            process_exit((int)r->ebx);          /* Never returns */
            break;

        case SYS_WRITE:
            r->eax = (uint32_t)sys_write((const char *)r->ebx);
            break;

        case SYS_GETPID:
            r->eax = process_getpid();
            break;

        case SYS_FORK:
            r->eax = (uint32_t)sys_fork(r);
            break;

        case SYS_YIELD:
            r->eax = 0;
            scheduler_yield();
            break;

        case SYS_SLEEP:
            r->eax = 0;
            process_sleep(r->ebx);
            break;

        case SYS_GETPPID:
            r->eax = process_getppid();
            break;

        case SYS_WAIT: {
            int status = 0;
            int pid = process_wait(&status);
            if (r->ebx) {
                *(int *)r->ebx = status;
            }
            r->eax = (uint32_t)pid;
            break;
        }

        default:
            r->eax = (uint32_t)-1;
            break;
    }
}
