/*
 * OpenOS - Kernel Panic Handler Implementation
 * 
 * Handles unrecoverable kernel errors with detailed diagnostic information.
 */

#include "panic.h"
#include "../drivers/console.h"

/* Kernel panic - halts the system with an error message */
void kernel_panic(const char* message) {
    __asm__ __volatile__("cli");  /* Disable interrupts */
    
    console_set_color(0x0F, 0x04);  /* White text on red background */
    console_write("\n\n*** KERNEL PANIC ***\n");
    console_write(message);
    console_write("\n\nSystem halted.\n");
    
    /* Halt the CPU */
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

/* Kernel panic with additional context information */
void kernel_panic_ext(const char* message, const char* file, int line) {
    __asm__ __volatile__("cli");  /* Disable interrupts */
    
    console_set_color(0x0F, 0x04);  /* White text on red background */
    console_write("\n\n*** KERNEL PANIC ***\n");
    console_write(message);
    console_write("\n\nFile: ");
    console_write(file);
    console_write("\n");
    /* Note: Line number formatting would require sprintf, which we don't have yet */
    console_write("System halted.\n");
    
    /* Halt the CPU */
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
