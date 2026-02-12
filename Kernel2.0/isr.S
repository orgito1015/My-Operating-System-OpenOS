/*
 * OpenOS - Interrupt Service Routines (Assembly)
 */

/* Kernel segment selectors */
.set KERNEL_DATA_SEGMENT, 0x10

.section .text

/* External C handler */
.extern keyboard_handler

/* IRQ1 (Keyboard) handler */
.global irq1_handler
.type irq1_handler, @function
irq1_handler:
    /* Save all general purpose registers */
    pusha
    
    /* Save segment registers */
    push %ds
    push %es
    push %fs
    push %gs
    
    /* Load kernel data segment */
    mov $KERNEL_DATA_SEGMENT, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    /* Call C keyboard handler */
    call keyboard_handler
    
    /* Restore segment registers */
    pop %gs
    pop %fs
    pop %es
    pop %ds
    
    /* Restore general purpose registers */
    popa
    
    /* Return from interrupt */
    iret

/* IDT load function */
.global idt_load
.type idt_load, @function
idt_load:
    mov 4(%esp), %eax
    lidt (%eax)
    ret
