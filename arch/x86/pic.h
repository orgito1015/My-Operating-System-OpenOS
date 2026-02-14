/*
 * OpenOS - Programmable Interrupt Controller (PIC)
 */

#ifndef OPENOS_ARCH_X86_PIC_H
#define OPENOS_ARCH_X86_PIC_H

#include <stdint.h>
#include "ports.h"

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

#endif /* OPENOS_ARCH_X86_PIC_H */
