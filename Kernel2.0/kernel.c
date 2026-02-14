/*
 * OpenOS - Step 0
 *
 * Minimal freestanding kernel entry.
 * Runs in 32-bit protected mode, writes to VGA text buffer,
 * and then halts the CPU in a loop.
 */

#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "pic.h"
#include "isr.h"
#include "keyboard.h"
#include "exceptions.h"
#include "timer.h"
/* #include "pmm.h" */  /* TODO: Uncomment when Multiboot info is passed */

/* VGA text mode constants */
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t*)0xB8000)

/* GDT segment selectors */
#define KERNEL_CODE_SEGMENT 0x08
#define KERNEL_DATA_SEGMENT 0x10

/* IDT gate flags */
#define IDT_GATE_PRESENT    0x80
#define IDT_GATE_INT32      0x0E
#define IDT_FLAGS_KERNEL    (IDT_GATE_PRESENT | IDT_GATE_INT32)  /* 0x8E */

static uint16_t* const vga_buf = VGA_MEMORY;
static size_t term_row = 0;
static size_t term_col = 0;
static uint8_t term_color = 0x0F; /* white on black */

static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)(uint8_t)c | ((uint16_t)color << 8);
}

static void terminal_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buf[y * VGA_WIDTH + x] = vga_entry(' ', term_color);
        }
    }
    term_row = 0;
    term_col = 0;
}

static void terminal_scroll(void) {
    /* Scroll up by moving all lines up by one */
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buf[y * VGA_WIDTH + x] = vga_buf[(y + 1) * VGA_WIDTH + x];
        }
    }
    /* Clear the last line */
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        vga_buf[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', term_color);
    }
}

void terminal_backspace(void) {
    if (term_col > 0) {
        term_col--;
    } else if (term_row > 0) {
        term_row--;
        term_col = VGA_WIDTH - 1;
    }
    vga_buf[term_row * VGA_WIDTH + term_col] = vga_entry(' ', term_color);
}

void terminal_put_char(char c) {
    if (c == '\n') {
        term_col = 0;
        term_row++;
        if (term_row >= VGA_HEIGHT) {
            terminal_scroll();
            term_row = VGA_HEIGHT - 1;
        }
        return;
    }

    vga_buf[term_row * VGA_WIDTH + term_col] = vga_entry(c, term_color);
    term_col++;
    if (term_col >= VGA_WIDTH) {
        term_col = 0;
        term_row++;
        if (term_row >= VGA_HEIGHT) {
            terminal_scroll();
            term_row = VGA_HEIGHT - 1;
        }
    }
}

void terminal_write(const char* s) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        terminal_put_char(s[i]);
    }
}

/* Kernel entry point called from boot.S */
void kmain(void) {
    terminal_clear();
    terminal_write("OpenOS - Advanced Educational Kernel\n");
    terminal_write("====================================\n");
    terminal_write("Running in 32-bit protected mode.\n\n");

    /* Initialize IDT */
    terminal_write("[1/5] Initializing IDT...\n");
    idt_init();
    
    /* Install exception handlers */
    terminal_write("[2/5] Installing exception handlers...\n");
    exceptions_init();
    
    /* Initialize PIC */
    terminal_write("[3/5] Initializing PIC...\n");
    pic_init();
    
    /* Initialize timer (100 Hz) */
    terminal_write("[4/5] Initializing timer...\n");
    timer_init(100);
    idt_set_gate(0x20, (uint32_t)irq0_handler, KERNEL_CODE_SEGMENT, IDT_FLAGS_KERNEL);
    
    /* Install keyboard interrupt handler (IRQ1 = interrupt 0x21) */
    terminal_write("[5/5] Initializing keyboard...\n");
    idt_set_gate(0x21, (uint32_t)irq1_handler, KERNEL_CODE_SEGMENT, IDT_FLAGS_KERNEL);
    
    /* Initialize keyboard */
    keyboard_init();
    
    /* TODO: When Multiboot info is passed to kmain(), uncomment these lines:
     * terminal_write("[6/7] Initializing physical memory...\n");
     * pmm_init(mboot);
     * 
     * terminal_write("[7/7] Initializing virtual memory...\n");
     * vmm_init();
     */
    
    /* Enable interrupts */
    __asm__ __volatile__("sti");
    
    terminal_write("\n*** System Ready ***\n");
    terminal_write("- Exception handling: Active\n");
    terminal_write("- Timer interrupts: 100 Hz\n");
    terminal_write("- Keyboard: Ready\n\n");
    terminal_write("Type commands and press Enter!\n\n");
    
    /* Interactive prompt loop */
    char input[256];
    while (1) {
        terminal_write("OpenOS> ");
        keyboard_get_line(input, sizeof(input));
        terminal_write("You typed: ");
        terminal_write(input);
        terminal_write("\n");
    }
}
