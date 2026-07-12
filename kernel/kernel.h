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
#include "../fs/vfs.h"
#include "../include/multiboot.h"

/* GDT segment selectors (kernel's own GDT, see arch/x86/gdt.h) */
#define KERNEL_CODE_SEGMENT 0x08
#define KERNEL_DATA_SEGMENT 0x10

/* IDT gate flags */
#define IDT_GATE_PRESENT    0x80
#define IDT_GATE_INT32      0x0E
#define IDT_GATE_DPL3       0x60
#define IDT_FLAGS_KERNEL    (IDT_GATE_PRESENT | IDT_GATE_INT32)                 /* 0x8E */
#define IDT_FLAGS_USER      (IDT_GATE_PRESENT | IDT_GATE_INT32 | IDT_GATE_DPL3) /* 0xEE */

/* Current working directory */
extern vfs_node_t* current_directory;

/* Kernel main entry point */
void kmain(struct multiboot_info *mboot);

/* Interactive shell process entry (launched from kmain) */
void shell_task(void *arg);

/* Current directory functions */
vfs_node_t* kernel_get_current_directory(void);
void kernel_set_current_directory(vfs_node_t* dir);

#endif /* OPENOS_KERNEL_H */
