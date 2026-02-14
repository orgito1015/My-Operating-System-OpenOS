/*
 * OpenOS - Console Driver Implementation
 * 
 * VGA text mode console for kernel output.
 */

#include "console.h"
#include <stdint.h>
#include <stddef.h>

/* VGA memory address */
#define VGA_MEMORY ((uint16_t*)0xB8000)

static uint16_t* const vga_buf = VGA_MEMORY;
static size_t term_row = 0;
static size_t term_col = 0;
static uint8_t term_color = 0x0F; /* white on black */

/* Create a VGA entry with character and color */
static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)(uint8_t)c | ((uint16_t)color << 8);
}

/* Scroll the terminal up by one line */
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

/* Initialize console */
void console_init(void) {
    console_clear();
}

/* Clear console */
void console_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buf[y * VGA_WIDTH + x] = vga_entry(' ', term_color);
        }
    }
    term_row = 0;
    term_col = 0;
}

/* Backspace operation */
void console_backspace(void) {
    if (term_col > 0) {
        term_col--;
    } else if (term_row > 0) {
        term_row--;
        term_col = VGA_WIDTH - 1;
    }
    vga_buf[term_row * VGA_WIDTH + term_col] = vga_entry(' ', term_color);
}

/* Put a character on the console */
void console_put_char(char c) {
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

/* Write a string to the console */
void console_write(const char* s) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        console_put_char(s[i]);
    }
}

/* Set console color */
void console_set_color(uint8_t fg, uint8_t bg) {
    term_color = fg | (bg << 4);
}

/* Legacy function aliases for backwards compatibility */
void terminal_put_char(char c) {
    console_put_char(c);
}

void terminal_backspace(void) {
    console_backspace();
}
