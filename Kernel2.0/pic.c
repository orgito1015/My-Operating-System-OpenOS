/*
 * OpenOS - Programmable Interrupt Controller (PIC) Implementation
 */

#include "pic.h"

/* I/O wait function */
static void io_wait(void) {
    /* Port 0x80 is used for 'checkpoints' during POST */
    /* Writing to this port should take approximately 1 microsecond */
    outb(0x80, 0);
}

/* Initialize and remap the PIC */
void pic_init(void) {
    /* Start initialization sequence (ICW1) */
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();

    /* ICW2: Set interrupt vector offsets */
    /* Master PIC: IRQ 0-7 mapped to interrupts 0x20-0x27 */
    outb(PIC1_DATA, 0x20);
    io_wait();
    /* Slave PIC: IRQ 8-15 mapped to interrupts 0x28-0x2F */
    outb(PIC2_DATA, 0x28);
    io_wait();

    /* ICW3: Tell Master PIC there's a slave at IRQ2 */
    outb(PIC1_DATA, 0x04);
    io_wait();
    /* ICW3: Tell Slave PIC its cascade identity (IRQ2) */
    outb(PIC2_DATA, 0x02);
    io_wait();

    /* ICW4: Set 8086 mode */
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    /* Mask all interrupts initially - let drivers enable what they need */
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

/* Send End Of Interrupt signal */
void pic_send_eoi(uint8_t irq) {
    /* If IRQ came from slave PIC (IRQ 8-15), send EOI to both PICs */
    if (irq >= 8) {
        outb(PIC2_CMD, PIC_EOI);
    }
    /* Always send EOI to master PIC (for IRQ 0-7 and slave IRQs) */
    outb(PIC1_CMD, PIC_EOI);
}
