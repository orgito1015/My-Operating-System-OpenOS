/*
 * OpenOS - Virtual Memory Manager (VMM)
 * Provides virtual memory management with paging support
 */

#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>

/* Page size constants */
#define PAGE_SIZE           4096
#define PAGE_TABLE_ENTRIES  1024
#define PAGE_DIR_ENTRIES    1024

/* Page flags */
#define PTE_PRESENT         (1 << 0)
#define PTE_WRITABLE        (1 << 1)
#define PTE_USER            (1 << 2)
#define PTE_WRITETHROUGH    (1 << 3)
#define PTE_NOCACHE         (1 << 4)
#define PTE_ACCESSED        (1 << 5)
#define PTE_DIRTY           (1 << 6)
#define PTE_PAT             (1 << 7)
#define PTE_GLOBAL          (1 << 8)

/* Kernel virtual base address (higher-half kernel) */
#define KERNEL_VIRTUAL_BASE 0xC0000000

/* Physical to virtual address conversion macros */
#define PHYS_TO_VIRT(addr)  ((void*)((uint32_t)(addr) + KERNEL_VIRTUAL_BASE))
#define VIRT_TO_PHYS(addr)  ((uint32_t)(addr) - KERNEL_VIRTUAL_BASE)

/* Page table structure */
struct page_table {
    uint32_t entries[PAGE_TABLE_ENTRIES];
} __attribute__((aligned(PAGE_SIZE)));

/* Page directory structure */
struct page_directory {
    uint32_t entries[PAGE_DIR_ENTRIES];
    struct page_table *tables[PAGE_DIR_ENTRIES];
} __attribute__((aligned(PAGE_SIZE)));

/* Initialize virtual memory management */
void vmm_init(void);

/* Create a new page directory */
struct page_directory *vmm_create_directory(void);

/* Destroy a page directory */
void vmm_destroy_directory(struct page_directory *dir);

/* Switch to a different page directory */
void vmm_switch_directory(struct page_directory *dir);

/* Map a virtual page to a physical frame */
int vmm_map_page(struct page_directory *dir, void *virt, uint32_t phys, uint32_t flags);

/* Unmap a virtual page */
void vmm_unmap_page(struct page_directory *dir, void *virt);

/* Get physical address for a virtual address */
uint32_t vmm_get_physical(struct page_directory *dir, void *virt);

/* Identity map a region (virtual address == physical address) */
void vmm_identity_map_region(struct page_directory *dir, void *start, size_t size, uint32_t flags);

/* Map a region of memory */
void vmm_map_region(struct page_directory *dir, void *virt, uint32_t phys, size_t size, uint32_t flags);

/* Page fault handler */
void vmm_page_fault_handler(void);

#endif /* VMM_H */
