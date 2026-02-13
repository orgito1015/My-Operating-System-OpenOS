/*
 * OpenOS - Virtual Memory Manager (VMM) Implementation
 */

#include "vmm.h"
#include "pmm.h"
#include <stddef.h>
#include <stdbool.h>

/* Current page directory */
static struct page_directory *current_directory = 0;

/* Kernel page directory */
static struct page_directory *kernel_directory = 0;

/* Helper macros for page directory/table indexing */
#define PD_INDEX(addr) (((uint32_t)(addr) >> 22) & 0x3FF)
#define PT_INDEX(addr) (((uint32_t)(addr) >> 12) & 0x3FF)
#define PAGE_ALIGN(addr) ((uint32_t)(addr) & 0xFFFFF000)

/* TLB flush for a single page */
static inline void tlb_flush_page(void *virt) {
    __asm__ __volatile__("invlpg (%0)" : : "r"(virt) : "memory");
}

/* TLB flush for entire address space */
static inline void tlb_flush_all(void) {
    uint32_t cr3;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3));
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(cr3));
}

/*
 * Get or create a page table for a virtual address
 * NOTE: This function assumes physical memory is identity-mapped (virt == phys)
 *       which is true during Phase 0. For higher-half kernel, this will need updating.
 */
static struct page_table *get_page_table(struct page_directory *dir, void *virt, bool create) {
    uint32_t pd_index = PD_INDEX(virt);
    
    /* Check if page table exists */
    if (dir->tables[pd_index] != NULL) {
        return dir->tables[pd_index];
    }
    
    /* Create new page table if requested */
    if (create) {
        /* Allocate physical page for page table */
        void *phys = pmm_alloc_page();
        if (phys == NULL) {
            return NULL;
        }
        
        /* Clear the page table */
        struct page_table *pt = (struct page_table *)phys;
        for (uint32_t i = 0; i < PAGE_TABLE_ENTRIES; i++) {
            pt->entries[i] = 0;
        }
        
        /* Store page table pointer */
        dir->tables[pd_index] = pt;
        
        /* Set page directory entry */
        dir->entries[pd_index] = ((uint32_t)phys & 0xFFFFF000) | 
                                  PTE_PRESENT | PTE_WRITABLE;
        
        return pt;
    }
    
    return NULL;
}

/*
 * Initialize the Virtual Memory Manager
 * NOTE: This assumes physical memory is identity-mapped during initialization.
 *       The first 4MB is identity-mapped to cover kernel code/data and VGA buffer.
 */
void vmm_init(void) {
    /* Allocate kernel page directory */
    void *dir_phys = pmm_alloc_page();
    if (dir_phys == NULL) {
        return;
    }
    
    kernel_directory = (struct page_directory *)dir_phys;
    
    /* Clear page directory */
    for (uint32_t i = 0; i < PAGE_DIR_ENTRIES; i++) {
        kernel_directory->entries[i] = 0;
        kernel_directory->tables[i] = NULL;
    }
    
    /* Identity map first 4MB (covers kernel and VGA) */
    vmm_identity_map_region(kernel_directory, 0, 0x400000, 
                           PTE_PRESENT | PTE_WRITABLE);
    
    /* Set as current directory */
    current_directory = kernel_directory;
    
    /* Load page directory into CR3 */
    vmm_switch_directory(kernel_directory);
}

/*
 * Create a new page directory
 */
struct page_directory *vmm_create_directory(void) {
    /* Allocate physical page for directory */
    void *dir_phys = pmm_alloc_page();
    if (dir_phys == NULL) {
        return NULL;
    }
    
    struct page_directory *dir = (struct page_directory *)dir_phys;
    
    /* Clear page directory */
    for (uint32_t i = 0; i < PAGE_DIR_ENTRIES; i++) {
        dir->entries[i] = 0;
        dir->tables[i] = NULL;
    }
    
    return dir;
}

/*
 * Destroy a page directory
 */
void vmm_destroy_directory(struct page_directory *dir) {
    if (dir == NULL) {
        return;
    }
    
    /* Free all page tables */
    for (uint32_t i = 0; i < PAGE_DIR_ENTRIES; i++) {
        if (dir->tables[i] != NULL) {
            pmm_free_page(dir->tables[i]);
        }
    }
    
    /* Free the directory itself */
    pmm_free_page(dir);
}

/*
 * Switch to a different page directory
 */
void vmm_switch_directory(struct page_directory *dir) {
    if (!dir) return;
    
    current_directory = dir;
    
    /* Load the page directory into CR3 */
    uint32_t phys_addr = (uint32_t)dir;
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(phys_addr));
}

/*
 * Map a virtual page to a physical frame
 */
int vmm_map_page(struct page_directory *dir, void *virt, uint32_t phys, uint32_t flags) {
    if (dir == NULL) {
        dir = current_directory;
    }
    
    /* Get or create page table */
    struct page_table *pt = get_page_table(dir, virt, true);
    if (pt == NULL) {
        return 0;
    }
    
    /* Get page table entry */
    uint32_t pt_index = PT_INDEX(virt);
    
    /* Map the page */
    pt->entries[pt_index] = (phys & 0xFFFFF000) | (flags & 0xFFF);
    
    /* Flush TLB for this page */
    tlb_flush_page(virt);
    
    return 1;
}

/*
 * Unmap a virtual page
 */
void vmm_unmap_page(struct page_directory *dir, void *virt) {
    if (dir == NULL) {
        dir = current_directory;
    }
    
    /* Get page table */
    struct page_table *pt = get_page_table(dir, virt, false);
    if (pt == NULL) {
        return;
    }
    
    /* Get page table entry */
    uint32_t pt_index = PT_INDEX(virt);
    
    /* Clear the page table entry */
    pt->entries[pt_index] = 0;
    
    /* Flush TLB for this page */
    tlb_flush_page(virt);
}

/*
 * Get physical address for a virtual address
 */
uint32_t vmm_get_physical(struct page_directory *dir, void *virt) {
    if (dir == NULL) {
        dir = current_directory;
    }
    
    /* Get page table */
    struct page_table *pt = get_page_table(dir, virt, false);
    if (pt == NULL) {
        return 0;
    }
    
    /* Get page table entry */
    uint32_t pt_index = PT_INDEX(virt);
    uint32_t pte = pt->entries[pt_index];
    
    /* Check if page is present */
    if (!(pte & PTE_PRESENT)) {
        return 0;
    }
    
    /* Return physical address with offset */
    return (pte & 0xFFFFF000) | ((uint32_t)virt & 0xFFF);
}

/*
 * Identity map a region (virtual address == physical address)
 */
void vmm_identity_map_region(struct page_directory *dir, void *start, size_t size, uint32_t flags) {
    if (dir == NULL) {
        dir = current_directory;
    }
    
    /* Align start address down to page boundary */
    uint32_t virt = PAGE_ALIGN((uint32_t)start);
    uint32_t end = ((uint32_t)start + size + PAGE_SIZE - 1) & 0xFFFFF000;
    
    /* Map each page in the region */
    while (virt < end) {
        vmm_map_page(dir, (void *)virt, virt, flags);
        virt += PAGE_SIZE;
    }
}

/*
 * Map a region of memory
 */
void vmm_map_region(struct page_directory *dir, void *virt, uint32_t phys, size_t size, uint32_t flags) {
    if (dir == NULL) {
        dir = current_directory;
    }
    
    /* Align addresses to page boundaries */
    uint32_t virt_addr = PAGE_ALIGN((uint32_t)virt);
    uint32_t phys_addr = PAGE_ALIGN(phys);
    uint32_t end = ((uint32_t)virt + size + PAGE_SIZE - 1) & 0xFFFFF000;
    
    /* Map each page in the region */
    while (virt_addr < end) {
        vmm_map_page(dir, (void *)virt_addr, phys_addr, flags);
        virt_addr += PAGE_SIZE;
        phys_addr += PAGE_SIZE;
    }
}

/*
 * Page fault handler
 */
void vmm_page_fault_handler(void) {
    /* Get the faulting address from CR2 */
    uint32_t faulting_address;
    __asm__ __volatile__("mov %%cr2, %0" : "=r"(faulting_address));
    
    /* This is handled by the exception system now */
    /* The page fault exception (14) will be caught by exception_handler */
    (void)faulting_address;
}
