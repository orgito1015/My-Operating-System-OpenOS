/*
 * OpenOS - Kernel Main Entry Point
 * 
 * This is the main entry point for the OpenOS kernel after boot.
 * It initializes all kernel subsystems and enters the main kernel loop.
 */

#ifndef OPENOS_KERNEL_H
#define OPENOS_KERNEL_H

#include <stdint.h>
#include <stddef.h>

/* GDT segment selectors (GRUB's GDT) */
#define KERNEL_CODE_SEGMENT 0x10   /* GRUB uses 0x10 for code */
#define KERNEL_DATA_SEGMENT 0x18   /* GRUB uses 0x18 for data */

/* IDT gate flags */
#define IDT_GATE_PRESENT    0x80
#define IDT_GATE_INT32      0x0E
#define IDT_FLAGS_KERNEL    (IDT_GATE_PRESENT | IDT_GATE_INT32)  /* 0x8E */

/* Kernel main entry point */
void kmain(void);

#endif /* OPENOS_KERNEL_H */
