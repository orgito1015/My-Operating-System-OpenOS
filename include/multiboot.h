/*
 * OpenOS - Multiboot Specification Structures
 * 
 * Defines structures for parsing multiboot information passed by bootloader.
 * Based on Multiboot Specification version 0.6.96
 */

#ifndef OPENOS_MULTIBOOT_H
#define OPENOS_MULTIBOOT_H

#include <stdint.h>

/* Multiboot header magic number */
#define MULTIBOOT_HEADER_MAGIC  0x1BADB002

/* Multiboot bootloader magic number */
#define MULTIBOOT_BOOTLOADER_MAGIC  0x2BADB002

/* Multiboot flags */
#define MULTIBOOT_FLAG_MEM          (1 << 0)  /* Memory info available */
#define MULTIBOOT_FLAG_DEVICE       (1 << 1)  /* Boot device info available */
#define MULTIBOOT_FLAG_CMDLINE      (1 << 2)  /* Command line available */
#define MULTIBOOT_FLAG_MODS         (1 << 3)  /* Modules available */
#define MULTIBOOT_FLAG_MMAP         (1 << 6)  /* Memory map available */

/* Multiboot memory map entry */
struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

/* Multiboot memory types */
#define MULTIBOOT_MEMORY_AVAILABLE        1
#define MULTIBOOT_MEMORY_RESERVED         2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS              4
#define MULTIBOOT_MEMORY_BADRAM           5

/* Multiboot info structure */
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} __attribute__((packed));

#endif /* OPENOS_MULTIBOOT_H */
