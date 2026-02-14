/*
 * OpenOS - Console Driver
 * 
 * Provides VGA text mode console interface for kernel output.
 */

#ifndef OPENOS_DRIVERS_CONSOLE_H
#define OPENOS_DRIVERS_CONSOLE_H

#include <stddef.h>
#include <stdint.h>

/* VGA text mode constants */
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

/* Initialize console */
void console_init(void);

/* Clear console */
void console_clear(void);

/* Put a character on the console */
void console_put_char(char c);

/* Write a string to the console */
void console_write(const char* s);

/* Backspace operation */
void console_backspace(void);

/* Set console color */
void console_set_color(uint8_t fg, uint8_t bg);

#endif /* OPENOS_DRIVERS_CONSOLE_H */
