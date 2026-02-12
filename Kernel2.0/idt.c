/*
 * OpenOS - Interrupt Descriptor Table (IDT) Implementation
 */

#include "idt.h"
#include <stdint.h>
#include <stddef.h>

#define IDT_ENTRIES 256

/* IDT array */
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

/* External assembly function to load IDT */
extern void idt_load(uint32_t);

/* Set an IDT gate */
void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].offset_high = (handler >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

/* Initialize the IDT */
void idt_init(void) {
    /* Clear the IDT - initialize all entries to zero */
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i].offset_low = 0;
        idt[i].offset_high = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
    }

    /* Set up the IDT pointer structure */
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base = (uint32_t)&idt;

    /* Load the IDT into the CPU */
    idt_load((uint32_t)&idtp);
}
