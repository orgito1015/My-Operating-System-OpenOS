/*
 * OpenOS - Serial Port (UART 16550) Driver
 *
 * Provides output over the first serial port (COM1, 0x3F8). The console
 * driver mirrors all screen output here, which makes the kernel usable
 * headless and lets QEMU capture a full boot/session log:
 *
 *     make debug      (serial routed to the terminal)
 *     make qemu-log   (serial + machine events written to qemu.log)
 *     qemu-system-i386 -cdrom openos.iso -serial stdio
 *
 * Output only for now; interrupt-driven input can be layered on later.
 */

#ifndef OPENOS_DRIVERS_SERIAL_H
#define OPENOS_DRIVERS_SERIAL_H

#include <stdint.h>
#include <stdbool.h>

/* COM1 base I/O port. */
#define SERIAL_COM1_BASE 0x3F8

/*
 * Initialize COM1 at 38400 baud, 8 data bits, no parity, 1 stop bit.
 * Performs the UART loopback self-test; if the port does not echo back
 * correctly (e.g. no UART present) serial output is disabled so writes
 * become harmless no-ops. Safe to call more than once.
 */
void serial_init(void);

/* True if serial_init() found a working UART. */
bool serial_is_available(void);

/* Write a single byte to COM1 (blocks until the TX holding register is free). */
void serial_write_char(char c);

/* Write a NUL-terminated string to COM1. '\n' is expanded to CR/LF. */
void serial_write(const char* s);

#endif /* OPENOS_DRIVERS_SERIAL_H */
