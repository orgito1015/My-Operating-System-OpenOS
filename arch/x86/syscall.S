/*
 * OpenOS - System Call Entry (int 0x80)
 *
 * Ring 3 code executes `int $0x80` with the syscall number in EAX and
 * arguments in EBX, ECX, EDX. The CPU switches to the kernel stack
 * (TSS.ESP0), pushes SS/ESP/EFLAGS/CS/EIP, and lands here.
 *
 * The register frame built below must stay in sync with `regs_t` in
 * kernel/syscall.h and with fork_child_return in context.S.
 */

.set KERNEL_DATA_SEGMENT, 0x10

.section .text

.extern syscall_dispatch

.global int80_handler
.type int80_handler, @function
int80_handler:
    pusha                       /* eax..edi (edi ends up lowest)      */

    push %ds
    push %es
    push %fs
    push %gs

    mov $KERNEL_DATA_SEGMENT, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push %esp                   /* regs_t* argument                   */
    call syscall_dispatch       /* return value written to regs->eax  */
    add $4, %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds

    popa
    iret
