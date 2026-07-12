/*
 * OpenOS - Process Management (Phase 1)
 *
 * Implements the process table, kernel threads, ring 3 processes,
 * exit/kill/wait/sleep. Scheduling policy lives in scheduler.c; the
 * low-level register save/restore lives in arch/x86/context.S.
 *
 * Kernel stack layout for a brand-new kernel thread (top downward):
 *
 *      kstack_top -> [ arg                ]  argument for entry()
 *                    [ process_thread_exit ]  entry()'s return address
 *                    [ entry              ]  context_switch ret target
 *                    [ EFLAGS = 0x202     ]  IF=1 once first scheduled
 *                    [ EBP = 0            ]
 *                    [ EBX = 0            ]
 *                    [ ESI = 0            ]
 *      esp --------> [ EDI = 0            ]
 *
 * context_switch() pops the callee-saved registers and EFLAGS, then
 * `ret` transfers control to entry() which sees a perfectly ordinary
 * cdecl frame: a return address (process_thread_exit) and one argument.
 * If entry() ever returns, it falls into process_thread_exit(), which
 * terminates the process cleanly.
 */

#include "process.h"
#include "scheduler.h"
#include "../memory/heap.h"
#include "../drivers/console.h"
#include "../drivers/timer.h"
#include "../arch/x86/gdt.h"
#include "../kernel/string.h"

/* From arch/x86/context.S */
extern void enter_user_mode(uint32_t entry, uint32_t user_stack_top);

/* Process table */
static process_t process_table[PROCESS_MAX];
static uint32_t  next_pid = 1;

/* Currently running process (set up by process_init/scheduler) */
process_t *current_process = 0;

/* ------------------------------------------------------------------ */
/* Helpers                                                              */
/* ------------------------------------------------------------------ */

static inline void irq_disable(void) { __asm__ __volatile__("cli"); }
static inline void irq_enable(void)  { __asm__ __volatile__("sti"); }

static void copy_name(process_t *p, const char *name) {
    size_t i = 0;
    if (name) {
        while (name[i] && i < PROCESS_NAME_LEN - 1) {
            p->name[i] = name[i];
            i++;
        }
    }
    p->name[i] = '\0';
}

/* Find a free table slot. Interrupts must be disabled. */
static process_t *alloc_pcb(void) {
    for (int i = 0; i < PROCESS_MAX; i++) {
        if (process_table[i].state == PROCESS_STATE_UNUSED) {
            process_t *p = &process_table[i];
            /* Clear the whole PCB */
            uint8_t *b = (uint8_t *)p;
            for (size_t j = 0; j < sizeof(*p); j++) b[j] = 0;
            p->pid = next_pid++;
            return p;
        }
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Initialization                                                       */
/* ------------------------------------------------------------------ */

void process_init(void) {
    for (int i = 0; i < PROCESS_MAX; i++) {
        process_table[i].state = PROCESS_STATE_UNUSED;
    }

    /*
     * Adopt the current boot context (kmain, running on the boot stack)
     * as PID 0: the idle process. It never sits in a ready queue; the
     * scheduler falls back to it when nothing else is runnable.
     */
    process_t *idle = &process_table[0];
    uint8_t *b = (uint8_t *)idle;
    for (size_t j = 0; j < sizeof(*idle); j++) b[j] = 0;

    idle->pid           = 0;
    idle->ppid          = 0;
    idle->state         = PROCESS_STATE_RUNNING;
    idle->priority      = PRIORITY_LOW;
    idle->base_priority = PRIORITY_LOW;
    idle->quantum_left  = SCHED_QUANTUM_TICKS;
    copy_name(idle, "idle");

    current_process = idle;

    console_write("Process: table initialized (64 slots), PID 0 = idle\n");
}

/* ------------------------------------------------------------------ */
/* Kernel threads                                                       */
/* ------------------------------------------------------------------ */

/* Landing pad if a thread's entry function returns. */
static void process_thread_exit(void) {
    process_exit(0);
}

process_t *process_create(const char *name, process_entry_t entry,
                          void *arg, uint8_t priority) {
    if (!entry || priority >= PRIORITY_LEVELS) {
        return 0;
    }

    uint8_t *stack = (uint8_t *)kmalloc(PROCESS_KSTACK_SIZE);
    if (!stack) {
        return 0;
    }

    irq_disable();

    process_t *p = alloc_pcb();
    if (!p) {
        irq_enable();
        kfree(stack);
        return 0;
    }

    copy_name(p, name);
    p->ppid          = current_process ? current_process->pid : 0;
    p->priority      = priority;
    p->base_priority = priority;
    p->quantum_left  = SCHED_QUANTUM_TICKS;
    p->kstack        = stack;
    p->kstack_top    = (uint32_t)stack + PROCESS_KSTACK_SIZE;
    p->is_user       = 0;

    /* Build the initial kernel stack frame (see file header comment). */
    uint32_t *sp = (uint32_t *)p->kstack_top;
    *(--sp) = (uint32_t)arg;                  /* entry's argument        */
    *(--sp) = (uint32_t)process_thread_exit;  /* entry's return address  */
    *(--sp) = (uint32_t)entry;                /* context_switch target   */
    *(--sp) = 0x202;                          /* EFLAGS (IF = 1)         */
    *(--sp) = 0;                              /* EBP                     */
    *(--sp) = 0;                              /* EBX                     */
    *(--sp) = 0;                              /* ESI                     */
    *(--sp) = 0;                              /* EDI                     */
    p->esp = (uint32_t)sp;

    p->state = PROCESS_STATE_READY;
    scheduler_enqueue(p);

    irq_enable();
    return p;
}

/* ------------------------------------------------------------------ */
/* User-mode processes                                                  */
/* ------------------------------------------------------------------ */

/*
 * Kernel-side trampoline for ring 3 processes: runs as a fresh kernel
 * thread, then drops to user mode. The user entry/stack were stashed in
 * the PCB by process_create_user().
 */
static void user_process_trampoline(void *arg) {
    (void)arg;
    process_t *self = current_process;
    enter_user_mode(self->user_entry, self->ustack_top);
    /* Not reached: user code exits via sys_exit(). */
}

process_t *process_create_user(const char *name, void (*entry)(void),
                               uint8_t priority) {
    uint8_t *ustack = (uint8_t *)kmalloc(PROCESS_USTACK_SIZE);
    if (!ustack) {
        return 0;
    }

    process_t *p = process_create(name, user_process_trampoline, 0, priority);
    if (!p) {
        kfree(ustack);
        return 0;
    }

    p->is_user    = 1;
    p->ustack     = ustack;
    /* Keep ESP 16-byte aligned and leave a small red zone at the top. */
    p->ustack_top = (((uint32_t)ustack + PROCESS_USTACK_SIZE) & ~0xFu) - 16;
    p->user_entry = (uint32_t)entry;
    return p;
}

/* ------------------------------------------------------------------ */
/* Exit / kill / wait / sleep                                           */
/* ------------------------------------------------------------------ */

void process_exit(int code) {
    irq_disable();

    process_t *self = current_process;

    self->exit_code = code;
    self->state     = PROCESS_STATE_ZOMBIE;

    /* User stack can be freed now; the kernel stack is still in use
     * until we have switched away, so the reaper frees it later. */
    if (self->ustack) {
        kfree(self->ustack);
        self->ustack = 0;
    }

    /* Wake a parent blocked in process_wait(). */
    process_t *parent = process_by_pid(self->ppid);
    if (parent && parent->parent_waiting &&
        parent->state == PROCESS_STATE_BLOCKED) {
        scheduler_unblock(parent);
    }

    schedule();

    /* Unreachable */
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}

int process_kill(uint32_t pid) {
    if (pid == 0) {
        return -1;  /* Cannot kill the idle process */
    }

    irq_disable();

    process_t *p = process_by_pid(pid);
    if (!p || p->state == PROCESS_STATE_UNUSED ||
        p->state == PROCESS_STATE_ZOMBIE) {
        irq_enable();
        return -1;
    }

    if (p == current_process) {
        irq_enable();
        process_exit(-1);           /* Never returns */
    }

    /* Remove from wherever it is queued and mark it a zombie. */
    scheduler_dequeue(p);
    p->exit_code = -1;
    p->state     = PROCESS_STATE_ZOMBIE;

    if (p->ustack) {
        kfree(p->ustack);
        p->ustack = 0;
    }

    /* Wake a waiting parent, as in process_exit(). */
    process_t *parent = process_by_pid(p->ppid);
    if (parent && parent->parent_waiting &&
        parent->state == PROCESS_STATE_BLOCKED) {
        scheduler_unblock(parent);
    }

    irq_enable();
    return 0;
}

int process_wait(int *status) {
    for (;;) {
        irq_disable();

        int have_children = 0;
        for (int i = 0; i < PROCESS_MAX; i++) {
            process_t *p = &process_table[i];
            if (p->state == PROCESS_STATE_UNUSED) continue;
            if (p->ppid != current_process->pid || p == current_process) continue;

            have_children = 1;
            if (p->state == PROCESS_STATE_ZOMBIE) {
                int pid = (int)p->pid;
                if (status) *status = p->exit_code;
                process_release(p);      /* Reap: frees stacks, slot   */
                irq_enable();
                return pid;
            }
        }

        if (!have_children) {
            irq_enable();
            return -1;
        }

        /* Block until a child exits. */
        current_process->parent_waiting = 1;
        current_process->state = PROCESS_STATE_BLOCKED;
        scheduler_block_current();
        current_process->parent_waiting = 0;
        irq_enable();
    }
}

void process_sleep(uint32_t ms) {
    if (ms == 0) {
        scheduler_yield();
        return;
    }

    irq_disable();

    /* 100 Hz timer -> 10 ms per tick; round up. */
    uint64_t ticks = (ms + 9) / 10;
    current_process->sleep_until = timer_get_ticks() + ticks;
    current_process->state = PROCESS_STATE_SLEEPING;
    scheduler_block_current();

    irq_enable();
}

/* ------------------------------------------------------------------ */
/* Queries / bookkeeping                                                */
/* ------------------------------------------------------------------ */

process_t *process_current(void)  { return current_process; }
uint32_t   process_getpid(void)   { return current_process ? current_process->pid : 0; }
uint32_t   process_getppid(void)  { return current_process ? current_process->ppid : 0; }

process_t *process_by_pid(uint32_t pid) {
    for (int i = 0; i < PROCESS_MAX; i++) {
        if (process_table[i].state != PROCESS_STATE_UNUSED &&
            process_table[i].pid == pid) {
            return &process_table[i];
        }
    }
    return 0;
}

process_t *process_table_entry(int index) {
    if (index < 0 || index >= PROCESS_MAX) return 0;
    return &process_table[index];
}

const char *process_state_name(process_state_t s) {
    switch (s) {
        case PROCESS_STATE_UNUSED:   return "unused";
        case PROCESS_STATE_READY:    return "ready";
        case PROCESS_STATE_RUNNING:  return "running";
        case PROCESS_STATE_BLOCKED:  return "blocked";
        case PROCESS_STATE_SLEEPING: return "sleeping";
        case PROCESS_STATE_ZOMBIE:   return "zombie";
        default:                     return "?";
    }
}

int process_count(void) {
    int n = 0;
    for (int i = 0; i < PROCESS_MAX; i++) {
        if (process_table[i].state != PROCESS_STATE_UNUSED) n++;
    }
    return n;
}

void process_release(process_t *p) {
    if (!p || p == current_process) return;

    if (p->kstack) {
        kfree(p->kstack);
        p->kstack = 0;
    }
    if (p->ustack) {
        kfree(p->ustack);
        p->ustack = 0;
    }
    p->state = PROCESS_STATE_UNUSED;
    p->pid   = 0;
}
