/*
 * OpenOS - Physical Memory Manager (PMM)
 * Manages physical memory frames using a bitmap allocator
 */

#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Multiboot memory map structure */
struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

/* Multiboot info structure (simplified) */
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

/* Multiboot memory types */
#define MULTIBOOT_MEMORY_AVAILABLE        1
#define MULTIBOOT_MEMORY_RESERVED         2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS              4
#define MULTIBOOT_MEMORY_BADRAM           5

/* Physical memory constants */
#define PMM_PAGE_SIZE       4096
#define PMM_BITMAP_SIZE     (1024 * 1024)  /* Support up to 4GB RAM */
#define PMM_LOW_MEMORY      0x100000       /* 1MB - reserve for BIOS/VGA */

/* Memory statistics structure */
struct pmm_stats {
    uint32_t total_pages;
    uint32_t used_pages;
    uint32_t free_pages;
    uint32_t total_memory_kb;
    uint32_t used_memory_kb;
    uint32_t free_memory_kb;
};

/* Initialize the physical memory manager */
void pmm_init(struct multiboot_info *mboot);

/* Allocate a physical page (returns physical address) */
void *pmm_alloc_page(void);

/* Free a physical page */
void pmm_free_page(void *page);

/* Mark a physical page as used */
void pmm_mark_used(void *page);

/* Mark a physical page as free */
void pmm_mark_free(void *page);

/* Get memory statistics */
void pmm_get_stats(struct pmm_stats *stats);

/* Check if a page is free */
bool pmm_is_page_free(void *page);

#endif /* PMM_H */
