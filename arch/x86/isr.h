/*
 * OpenOS - Interrupt Service Routines (ISRs)
 */

#ifndef OPENOS_ARCH_X86_ISR_H
#define OPENOS_ARCH_X86_ISR_H

/* IRQ handlers */
void irq0_handler(void);  /* Timer interrupt */
void irq1_handler(void);  /* Keyboard interrupt */

/* ISR installation */
void isr_install(void);

#endif /* OPENOS_ARCH_X86_ISR_H */
