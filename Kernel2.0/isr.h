/*
 * OpenOS - Interrupt Service Routines (ISRs)
 */

#ifndef ISR_H
#define ISR_H

/* IRQ handlers */
void irq1_handler(void);  /* Keyboard interrupt */

/* ISR installation */
void isr_install(void);

#endif /* ISR_H */
