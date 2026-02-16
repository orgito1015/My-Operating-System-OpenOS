/*
 * OpenOS - Kernel Main Entry Point
 * 
 * This is the main entry point for the OpenOS kernel after boot.
 * It initializes all kernel subsystems and enters the main kernel loop.
 */

#include "kernel.h"
#include "../arch/x86/idt.h"
#include "../arch/x86/pic.h"
#include "../arch/x86/isr.h"
#include "../arch/x86/exceptions.h"
#include "../drivers/keyboard.h"
#include "../drivers/timer.h"
#include "../drivers/console.h"
/* #include "../memory/pmm.h" */  /* TODO: Uncomment when Multiboot info is passed */
/* #include "../memory/vmm.h" */  /* TODO: Uncomment when Multiboot info is passed */

/* Kernel entry point called from boot.S */
void kmain(void) {
    /* Initialize console */
    console_init();
    
    console_write("OpenOS - Advanced Educational Kernel\n");
    console_write("====================================\n");
    console_write("Running in 32-bit protected mode.\n\n");

    /* Initialize IDT */
    console_write("[1/5] Initializing IDT...\n");
    idt_init();
    
    /* Install exception handlers */
    console_write("[2/5] Installing exception handlers...\n");
    exceptions_init();
    
    /* Initialize PIC */
    console_write("[3/5] Initializing PIC...\n");
    pic_init();
    
    /* Initialize timer (100 Hz) */
    console_write("[4/5] Initializing timer...\n");
    timer_init(100);
    
    /* Install timer interrupt handler (IRQ0 = interrupt 0x20) */
    idt_set_gate(0x20, (uint32_t)irq0_handler, KERNEL_CODE_SEGMENT, IDT_FLAGS_KERNEL);
    
    /* Install keyboard interrupt handler (IRQ1 = interrupt 0x21) */
    console_write("[5/5] Initializing keyboard...\n");
    idt_set_gate(0x21, (uint32_t)irq1_handler, KERNEL_CODE_SEGMENT, IDT_FLAGS_KERNEL);
    
    /* Initialize keyboard (this will unmask IRQ1) */
    keyboard_init();
    
    /* TODO: When Multiboot info is passed to kmain(), uncomment these lines:
     * console_write("[6/7] Initializing physical memory...\n");
     * pmm_init(mboot);
     * 
     * console_write("[7/7] Initializing virtual memory...\n");
     * vmm_init();
     */
    
    /* Enable interrupts */
    __asm__ __volatile__("sti");
    
    /* Now that interrupts are enabled, unmask the timer IRQ */
    pic_unmask_irq(0);
    
    console_write("\n*** System Ready ***\n");
    console_write("- Exception handling: Active\n");
    console_write("- Timer interrupts: 100 Hz\n");
    console_write("- Keyboard: Ready\n\n");
    console_write("Type commands and press Enter!\n\n");
    
    /* Interactive prompt loop */
    char input[256];
    while (1) {
        console_write("OpenOS> ");
        keyboard_get_line(input, sizeof(input));
        console_write("You typed: ");
        console_write(input);
        console_write("\n");
    }
}
