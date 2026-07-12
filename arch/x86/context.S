/*
 * OpenOS - Context Switching and Privilege Transitions (Assembly)
 *
 * Provides:
 *   gdt_flush        - load the kernel GDT and reload segment registers
 *   tss_flush        - load the TSS selector into the task register
 *   context_switch   - save/restore kernel execution contexts
 *   enter_user_mode  - iret into ring 3 for the first time
 *   fork_child_return- child side of fork(): unwind a syscall frame
 */

.set KERNEL_CODE_SEGMENT, 0x08
.set KERNEL_DATA_SEGMENT, 0x10
.set TSS_SELECTOR,        0x28
.set USER_CODE_SELECTOR,  0x1B     /* 0x18 | RPL 3 */
.set USER_DATA_SELECTOR,  0x23     /* 0x20 | RPL 3 */

.section .text

/*
 * void gdt_flush(uint32_t gdtp_addr)
 * Loads the GDT, reloads CS with a far jump, and reloads data segments.
 */
.global gdt_flush
.type gdt_flush, @function
gdt_flush:
    mov 4(%esp), %eax
    lgdt (%eax)

    mov $KERNEL_DATA_SEGMENT, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    /* Far jump to reload CS */
    ljmp $KERNEL_CODE_SEGMENT, $.Lflush_done
.Lflush_done:
    ret

/*
 * void tss_flush(void)
 * Loads the TSS selector into the task register.
 */
.global tss_flush
.type tss_flush, @function
tss_flush:
    mov $TSS_SELECTOR, %ax
    ltr %ax
    ret

/*
 * void context_switch(uint32_t *old_esp, uint32_t new_esp)
 *
 * Saves the callee-saved register context of the current process on its
 * own kernel stack, records the resulting stack pointer through old_esp,
 * switches to the new process's saved stack pointer, restores its
 * context, and returns into it.
 *
 * Saved frame layout (from the new ESP upward):
 *   [edi][esi][ebx][ebp][eflags][return address]
 *
 * Newly created processes get a hand-built frame of the same shape so
 * that "restoring" them for the first time falls through into their
 * entry trampoline.
 */
.global context_switch
.type context_switch, @function
context_switch:
    pushfl                      /* Save EFLAGS (incl. IF state)          */
    push %ebp
    push %ebx
    push %esi
    push %edi

    mov 24(%esp), %eax          /* arg 1: uint32_t *old_esp              */
    mov %esp, (%eax)            /* *old_esp = current ESP                */

    mov 28(%esp), %esp          /* arg 2: new_esp -> becomes our stack   */

    pop %edi
    pop %esi
    pop %ebx
    pop %ebp
    popfl
    ret

/*
 * void enter_user_mode(uint32_t entry, uint32_t user_stack_top)
 *
 * Switches the CPU to ring 3 by building an inter-privilege IRET frame:
 *   SS(user), ESP(user), EFLAGS(IF=1), CS(user), EIP(entry)
 * Never returns. Called on a process's own kernel stack; the TSS ESP0
 * must already point at this kernel stack so later interrupts re-enter
 * ring 0 correctly.
 */
.global enter_user_mode
.type enter_user_mode, @function
enter_user_mode:
    mov 4(%esp), %ecx           /* entry point            */
    mov 8(%esp), %edx           /* user stack top         */

    mov $USER_DATA_SELECTOR, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push $USER_DATA_SELECTOR    /* SS                     */
    push %edx                   /* ESP                    */
    push $0x202                 /* EFLAGS: IF=1           */
    push $USER_CODE_SELECTOR    /* CS                     */
    push %ecx                   /* EIP                    */
    iret

/*
 * fork_child_return
 *
 * First code a forked child executes in kernel mode. The child's kernel
 * stack was constructed by sys_fork() so that ESP points at a copied
 * syscall register frame (with EAX = 0). Unwind it exactly like the
 * int 0x80 stub epilogue and IRET back to user mode.
 */
.global fork_child_return
.type fork_child_return, @function
fork_child_return:
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    iret
