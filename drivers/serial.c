/*
 * OpenOS - Serial Port (UART 16550) Driver Implementation
 */

#include "serial.h"
#include "../arch/x86/ports.h"

/* 16550 UART register offsets from the base port. */
#define UART_DATA          0  /* DLAB=0: RX/TX buffer                       */
#define UART_INT_ENABLE    1  /* DLAB=0: interrupt enable                   */
#define UART_DIVISOR_LO    0  /* DLAB=1: divisor latch low byte             */
#define UART_DIVISOR_HI    1  /* DLAB=1: divisor latch high byte            */
#define UART_FIFO_CTRL     2  /* FIFO control                              */
#define UART_LINE_CTRL     3  /* line control (DLAB, word length, parity)  */
#define UART_MODEM_CTRL    4  /* modem control                             */
#define UART_LINE_STATUS   5  /* line status                               */

/* Line status register bits. */
#define LSR_TX_EMPTY       0x20  /* transmit holding register empty */

static bool serial_available = false;

void serial_init(void) {
    const uint16_t base = SERIAL_COM1_BASE;

    outb(base + UART_INT_ENABLE, 0x00);  /* Disable all interrupts          */
    outb(base + UART_LINE_CTRL,  0x80);  /* Enable DLAB (set baud divisor)  */
    outb(base + UART_DIVISOR_LO, 0x03);  /* Divisor = 3 -> 38400 baud (lo)  */
    outb(base + UART_DIVISOR_HI, 0x00);  /*                            (hi) */
    outb(base + UART_LINE_CTRL,  0x03);  /* 8 bits, no parity, 1 stop; DLAB off */
    outb(base + UART_FIFO_CTRL,  0xC7);  /* Enable FIFO, clear, 14-byte threshold */
    outb(base + UART_MODEM_CTRL, 0x0B);  /* IRQs enabled, RTS/DSR set        */

    /* UART loopback self-test: put the chip in loopback, send a byte, and
     * verify it comes back. If it does not, assume there is no usable UART. */
    outb(base + UART_MODEM_CTRL, 0x1E);  /* loopback mode */
    outb(base + UART_DATA, 0xAE);        /* test byte     */
    if (inb(base + UART_DATA) != 0xAE) {
        serial_available = false;
        return;
    }

    /* Restore normal operation. */
    outb(base + UART_MODEM_CTRL, 0x0F);
    serial_available = true;
}

bool serial_is_available(void) {
    return serial_available;
}

static inline int tx_ready(void) {
    return inb(SERIAL_COM1_BASE + UART_LINE_STATUS) & LSR_TX_EMPTY;
}

void serial_write_char(char c) {
    if (!serial_available) {
        return;
    }
    while (!tx_ready()) {
        /* spin until the holding register is free */
    }
    outb(SERIAL_COM1_BASE + UART_DATA, (uint8_t)c);
}

void serial_write(const char* s) {
    if (!serial_available || s == 0) {
        return;
    }
    for (uint32_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == '\n') {
            serial_write_char('\r');
        }
        serial_write_char(s[i]);
    }
}
