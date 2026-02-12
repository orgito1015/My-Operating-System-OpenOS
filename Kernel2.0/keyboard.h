/*
 * OpenOS - Keyboard Driver
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stddef.h>

/* Keyboard I/O port */
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

/* Initialize keyboard */
void keyboard_init(void);

/* Keyboard interrupt handler (called from ISR) */
void keyboard_handler(void);

/* Get a line of input (blocking) */
void keyboard_get_line(char* buffer, size_t max_len);

#endif /* KEYBOARD_H */
