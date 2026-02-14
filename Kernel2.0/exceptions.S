/*
 * OpenOS - Exception Handler Assembly Stubs
 * These stubs handle CPU exceptions and call the C exception handler
 */

.section .text

/* External C handler */
.extern exception_handler

/* Kernel data segment selector */
.set KERNEL_DATA_SEGMENT, 0x10

/* 
 * Exception stub macro for exceptions WITHOUT error code
 * The CPU doesn't push an error code, so we push a dummy 0
 */
.macro EXCEPTION_STUB_NO_ERR num
.global exception_\num
.type exception_\num, @function
exception_\num:
    push $0              /* Push dummy error code */
    push $\num           /* Push exception number */
    jmp exception_common
.endm

/* 
 * Exception stub macro for exceptions WITH error code
 * The CPU already pushed an error code
 */
.macro EXCEPTION_STUB_ERR num
.global exception_\num
.type exception_\num, @function
exception_\num:
    push $\num           /* Push exception number */
    jmp exception_common
.endm

/* Define all 32 exception handlers */
EXCEPTION_STUB_NO_ERR 0   /* Divide by zero */
EXCEPTION_STUB_NO_ERR 1   /* Debug */
EXCEPTION_STUB_NO_ERR 2   /* Non-maskable interrupt */
EXCEPTION_STUB_NO_ERR 3   /* Breakpoint */
EXCEPTION_STUB_NO_ERR 4   /* Overflow */
EXCEPTION_STUB_NO_ERR 5   /* Bound range exceeded */
EXCEPTION_STUB_NO_ERR 6   /* Invalid opcode */
EXCEPTION_STUB_NO_ERR 7   /* Device not available */
EXCEPTION_STUB_ERR    8   /* Double fault (has error code) */
EXCEPTION_STUB_NO_ERR 9   /* Coprocessor segment overrun */
EXCEPTION_STUB_ERR    10  /* Invalid TSS (has error code) */
EXCEPTION_STUB_ERR    11  /* Segment not present (has error code) */
EXCEPTION_STUB_ERR    12  /* Stack segment fault (has error code) */
EXCEPTION_STUB_ERR    13  /* General protection fault (has error code) */
EXCEPTION_STUB_ERR    14  /* Page fault (has error code) */
EXCEPTION_STUB_NO_ERR 15  /* Reserved */
EXCEPTION_STUB_NO_ERR 16  /* x87 FPU error */
EXCEPTION_STUB_ERR    17  /* Alignment check (has error code) */
EXCEPTION_STUB_NO_ERR 18  /* Machine check */
EXCEPTION_STUB_NO_ERR 19  /* SIMD floating point exception */
EXCEPTION_STUB_NO_ERR 20  /* Virtualization exception */
EXCEPTION_STUB_ERR    21  /* Control protection exception (has error code) */
EXCEPTION_STUB_NO_ERR 22  /* Reserved */
EXCEPTION_STUB_NO_ERR 23  /* Reserved */
EXCEPTION_STUB_NO_ERR 24  /* Reserved */
EXCEPTION_STUB_NO_ERR 25  /* Reserved */
EXCEPTION_STUB_NO_ERR 26  /* Reserved */
EXCEPTION_STUB_NO_ERR 27  /* Reserved */
EXCEPTION_STUB_NO_ERR 28  /* Hypervisor injection exception */
EXCEPTION_STUB_NO_ERR 29  /* VMM communication exception */
EXCEPTION_STUB_ERR    30  /* Security exception (has error code) */
EXCEPTION_STUB_NO_ERR 31  /* Reserved */

/*
 * Common exception handler
 * Saves all registers and calls C exception handler
 */
exception_common:
    /* Save all general purpose registers */
    pusha
    
    /* Save data segment selector */
    push %ds
    
    /* Load kernel data segment */
    mov $KERNEL_DATA_SEGMENT, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    /* Call C exception handler with pointer to register structure */
    push %esp
    call exception_handler
    add $4, %esp
    
    /* Restore data segment selector */
    pop %ds
    
    /* Restore all general purpose registers */
    popa
    
    /* Remove error code and exception number from stack */
    add $8, %esp
    
    /* Return from interrupt */
    iret
