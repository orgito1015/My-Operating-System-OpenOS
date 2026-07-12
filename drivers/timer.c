/*
 * OpenOS - Programmable Interval Timer Implementation
 */

#include "timer.h"
#include "../arch/x86/pic.h"
#include "../arch/x86/ports.h"
#include "../process/scheduler.h"

/* System tick counter */
static volatile uint64_t system_ticks = 0;

/* Timer frequency in Hz */
static uint32_t timer_frequency = 0;

/*
 * Initialize the timer with the specified frequency
 * Note: This function configures the PIT hardware but does NOT
 * enable the IRQ in the PIC. Call pic_unmask_irq(0) separately
 * after setting up the IDT handler to avoid race conditions.
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
    
    /* Do NOT enable interrupt here - let the kernel do it after
     * the IDT handler is registered to ensure proper initialization order */
}

/*
 * Timer interrupt handler
 * Called from IRQ0 assembly stub
 *
 * The EOI is sent BEFORE invoking the scheduler: scheduler_tick() may
 * context-switch away and not return to this frame for a while, and
 * the PIC must be able to deliver further timer interrupts to the
 * process we switch to.
 */
void timer_handler(void) {
    system_ticks++;
    
    /* Send EOI to PIC */
    pic_send_eoi(0);

    /* Drive preemptive scheduling (no-op until scheduler_start()). */
    scheduler_tick();
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
