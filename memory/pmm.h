/*
 * OpenOS - Physical Memory Manager (PMM)
 * Manages physical memory frames using a bitmap allocator
 */

#ifndef OPENOS_MEMORY_PMM_H
#define OPENOS_MEMORY_PMM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../include/multiboot.h"

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

#endif /* OPENOS_MEMORY_PMM_H */
