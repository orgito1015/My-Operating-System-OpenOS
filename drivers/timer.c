/*
 * OpenOS - Programmable Interval Timer Implementation
 */

#include "timer.h"
#include "../arch/x86/pic.h"
#include "../arch/x86/ports.h"

/* System tick counter */
static volatile uint64_t system_ticks = 0;

/* Timer frequency in Hz */
static uint32_t timer_frequency = 0;

/*
 * Initialize the timer with the specified frequency
 */
void timer_init(uint32_t frequency) {
    timer_frequency = frequency;
    
    /* Calculate divisor for desired frequency */
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;
    
    /* Send command byte: Channel 0, Lo/Hi byte access, Rate generator mode */
    outb(PIT_COMMAND, 0x36);
    
    /* Send divisor (low byte then high byte) */
    outb(PIT_CHANNEL0_DATA, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_DATA, (uint8_t)((divisor >> 8) & 0xFF));
    
    /* Reset tick counter */
    system_ticks = 0;
    
    /* Enable timer interrupt (IRQ0) in PIC */
    uint8_t mask = inb(PIC1_DATA);
    mask &= ~(1 << 0);  /* Clear bit 0 to enable IRQ0 */
    outb(PIC1_DATA, mask);
}

/*
 * Timer interrupt handler
 * Called from IRQ0 assembly stub
 */
void timer_handler(void) {
    system_ticks++;
    
    /* Send EOI to PIC */
    pic_send_eoi(0);
}

/*
 * Get the number of timer ticks since boot
 */
uint64_t timer_get_ticks(void) {
    return system_ticks;
}

/*
 * Get uptime in milliseconds
 * NOTE: Uptime will wrap around after ~49 days at 100Hz (32-bit overflow).
 *       This is acceptable for an educational OS in Phase 0.
 */
uint64_t timer_get_uptime_ms(void) {
    if (timer_frequency == 0) {
        return 0;
    }
    /* Use 32-bit arithmetic to avoid __udivdi3 */
    uint32_t ticks_low = (uint32_t)system_ticks;
    uint32_t ms = (ticks_low * 1000) / timer_frequency;
    return (uint64_t)ms;
}

/*
 * Wait for a specified number of ticks
 */
void timer_wait(uint32_t ticks) {
    uint64_t target = system_ticks + ticks;
    while (system_ticks < target) {
        __asm__ __volatile__("hlt");
    }
}
