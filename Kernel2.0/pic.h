/*
 * OpenOS - Programmable Interrupt Controller (PIC)
 */

#ifndef PIC_H
#define PIC_H

#include <stdint.h>

/* PIC I/O ports */
#define PIC1_CMD   0x20
#define PIC1_DATA  0x21
#define PIC2_CMD   0xA0
#define PIC2_DATA  0xA1

/* PIC commands */
#define PIC_EOI    0x20  /* End of Interrupt */

/* ICW1 */
#define ICW1_ICW4  0x01  /* ICW4 needed */
#define ICW1_INIT  0x10  /* Initialization */

/* ICW4 */
#define ICW4_8086  0x01  /* 8086 mode */

/* Initialize and remap PIC */
void pic_init(void);

/* Send End Of Interrupt signal */
void pic_send_eoi(uint8_t irq);

/* Port I/O helper functions */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#endif /* PIC_H */
