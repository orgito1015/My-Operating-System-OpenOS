/*
 * OpenOS - x86 Port I/O Functions
 * 
 * Provides inline functions for reading from and writing to x86 I/O ports.
 */

#ifndef OPENOS_ARCH_X86_PORTS_H
#define OPENOS_ARCH_X86_PORTS_H

#include <stdint.h>

/* Output a byte to an I/O port */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

/* Input a byte from an I/O port */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Output a word to an I/O port */
static inline void outw(uint16_t port, uint16_t val) {
    __asm__ __volatile__("outw %0, %1" : : "a"(val), "Nd"(port));
}

/* Input a word from an I/O port */
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ __volatile__("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Output a double word to an I/O port */
static inline void outl(uint16_t port, uint32_t val) {
    __asm__ __volatile__("outl %0, %1" : : "a"(val), "Nd"(port));
}

/* Input a double word from an I/O port */
static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ __volatile__("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* I/O wait - used for short delays between I/O operations */
static inline void io_wait(void) {
    /* Port 0x80 is used for 'checkpoints' during POST */
    /* Writing to this port should take approximately 1 microsecond */
    outb(0x80, 0);
}

#endif /* OPENOS_ARCH_X86_PORTS_H */
