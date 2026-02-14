/*
 * OpenOS - Programmable Interval Timer (PIT) Driver
 * Provides timer interrupts for scheduling and time keeping
 */

#ifndef OPENOS_DRIVERS_TIMER_H
#define OPENOS_DRIVERS_TIMER_H

#include <stdint.h>

/* PIT I/O ports */
#define PIT_CHANNEL0_DATA   0x40
#define PIT_CHANNEL1_DATA   0x41
#define PIT_CHANNEL2_DATA   0x42
#define PIT_COMMAND         0x43

/* PIT frequency */
#define PIT_BASE_FREQUENCY  1193182  /* Hz */

/* Initialize the timer with specified frequency */
void timer_init(uint32_t frequency);

/* Get the number of timer ticks since boot */
uint64_t timer_get_ticks(void);

/* Get uptime in milliseconds */
uint64_t timer_get_uptime_ms(void);

/* Wait for a specified number of ticks */
void timer_wait(uint32_t ticks);

/* Timer interrupt handler (called from IRQ0) */
void timer_handler(void);

#endif /* OPENOS_DRIVERS_TIMER_H */
