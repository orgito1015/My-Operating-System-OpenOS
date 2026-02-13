/*
 * OpenOS - Physical Memory Manager Implementation
 * Bitmap-based page frame allocator
 */

#include "pmm.h"
#include <stdint.h>

/* Bitmap to track page frame usage (1 bit per page) */
static uint8_t pmm_bitmap[PMM_BITMAP_SIZE];

/* Total number of physical pages in the system */
static uint32_t total_pages = 0;

/* Number of used pages */
static uint32_t used_pages = 0;

/* Highest physical address we've seen */
static uint64_t max_physical_address = 0;

/*
 * Set a bit in the bitmap (mark page as used)
 */
static inline void bitmap_set(uint32_t page) {
    uint32_t byte = page / 8;
    uint32_t bit = page % 8;
    if (byte < PMM_BITMAP_SIZE) {
        pmm_bitmap[byte] |= (1 << bit);
    }
}

/*
 * Clear a bit in the bitmap (mark page as free)
 */
static inline void bitmap_clear(uint32_t page) {
    uint32_t byte = page / 8;
    uint32_t bit = page % 8;
    if (byte < PMM_BITMAP_SIZE) {
        pmm_bitmap[byte] &= ~(1 << bit);
    }
}

/*
 * Test a bit in the bitmap (check if page is used)
 */
static inline bool bitmap_test(uint32_t page) {
    uint32_t byte = page / 8;
    uint32_t bit = page % 8;
    if (byte < PMM_BITMAP_SIZE) {
        return (pmm_bitmap[byte] & (1 << bit)) != 0;
    }
    return true;  /* Assume used if out of range */
}

/*
 * Initialize the physical memory manager
 */
void pmm_init(struct multiboot_info *mboot) {
    /* Initialize bitmap - mark all pages as used initially */
    for (uint32_t i = 0; i < PMM_BITMAP_SIZE; i++) {
        pmm_bitmap[i] = 0xFF;
    }
    
    /* Check if memory map is available */
    if (!(mboot->flags & 0x40)) {
        /* No memory map available - use basic memory info */
        uint32_t mem_kb = mboot->mem_lower + mboot->mem_upper;
        total_pages = (mem_kb * 1024) / PMM_PAGE_SIZE;
        max_physical_address = mem_kb * 1024;
        
        /* Mark all pages above 1MB as free */
        uint32_t start_page = PMM_LOW_MEMORY / PMM_PAGE_SIZE;
        for (uint32_t i = start_page; i < total_pages; i++) {
            bitmap_clear(i);
        }
        
        used_pages = start_page;
        return;
    }
    
    /* Parse multiboot memory map */
    struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *)mboot->mmap_addr;
    struct multiboot_mmap_entry *mmap_end = 
        (struct multiboot_mmap_entry *)(mboot->mmap_addr + mboot->mmap_length);
    
    /* First pass: determine total memory */
    while (mmap < mmap_end) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint64_t region_end = mmap->addr + mmap->len;
            if (region_end > max_physical_address) {
                max_physical_address = region_end;
            }
        }
        mmap = (struct multiboot_mmap_entry *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }
    
    /* Calculate total pages */
    total_pages = (uint32_t)(max_physical_address / PMM_PAGE_SIZE);
    if (total_pages > PMM_BITMAP_SIZE * 8) {
        total_pages = PMM_BITMAP_SIZE * 8;
    }
    
    /* Second pass: mark available memory regions as free */
    mmap = (struct multiboot_mmap_entry *)mboot->mmap_addr;
    while (mmap < mmap_end) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE && mmap->addr >= PMM_LOW_MEMORY) {
            /* Mark pages in this region as free */
            uint32_t start_page = (uint32_t)(mmap->addr / PMM_PAGE_SIZE);
            uint32_t end_page = (uint32_t)((mmap->addr + mmap->len) / PMM_PAGE_SIZE);
            
            for (uint32_t page = start_page; page < end_page && page < total_pages; page++) {
                bitmap_clear(page);
            }
        }
        mmap = (struct multiboot_mmap_entry *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }
    
    /* Count used pages */
    used_pages = 0;
    for (uint32_t i = 0; i < total_pages; i++) {
        if (bitmap_test(i)) {
            used_pages++;
        }
    }
}

/*
 * Allocate a physical page
 */
void *pmm_alloc_page(void) {
    /* Find first free page */
    for (uint32_t page = 0; page < total_pages; page++) {
        if (!bitmap_test(page)) {
            /* Found a free page */
            bitmap_set(page);
            used_pages++;
            return (void *)(page * PMM_PAGE_SIZE);
        }
    }
    
    /* No free pages available */
    return NULL;
}

/*
 * Free a physical page
 */
void pmm_free_page(void *page) {
    uint32_t page_num = (uint32_t)(uintptr_t)page / PMM_PAGE_SIZE;
    
    if (page_num < total_pages && bitmap_test(page_num)) {
        bitmap_clear(page_num);
        used_pages--;
    }
}

/*
 * Mark a physical page as used
 */
void pmm_mark_used(void *page) {
    uint32_t page_num = (uint32_t)(uintptr_t)page / PMM_PAGE_SIZE;
    
    if (page_num < total_pages && !bitmap_test(page_num)) {
        bitmap_set(page_num);
        used_pages++;
    }
}

/*
 * Mark a physical page as free
 */
void pmm_mark_free(void *page) {
    pmm_free_page(page);
}

/*
 * Check if a page is free
 */
bool pmm_is_page_free(void *page) {
    uint32_t page_num = (uint32_t)(uintptr_t)page / PMM_PAGE_SIZE;
    
    if (page_num >= total_pages) {
        return false;
    }
    
    return !bitmap_test(page_num);
}

/*
 * Get memory statistics
 */
void pmm_get_stats(struct pmm_stats *stats) {
    stats->total_pages = total_pages;
    stats->used_pages = used_pages;
    stats->free_pages = total_pages - used_pages;
    stats->total_memory_kb = (total_pages * PMM_PAGE_SIZE) / 1024;
    stats->used_memory_kb = (used_pages * PMM_PAGE_SIZE) / 1024;
    stats->free_memory_kb = stats->total_memory_kb - stats->used_memory_kb;
}
