/*
 * OpenOS - Global Descriptor Table (GDT) and Task State Segment (TSS)
 *
 * Phase 1 installs the kernel's own GDT instead of relying on the
 * bootloader's. The layout provides ring 0 and ring 3 segments plus
 * a TSS, which is required for ring 3 -> ring 0 transitions (the CPU
 * loads SS0:ESP0 from the TSS when an interrupt or syscall arrives
 * while executing user code).
 *
 * Selector layout:
 *   0x00  null
 *   0x08  kernel code   (ring 0)
 *   0x10  kernel data   (ring 0)
 *   0x18  user code     (ring 3, use selector 0x1B = 0x18 | RPL 3)
 *   0x20  user data     (ring 3, use selector 0x23 = 0x20 | RPL 3)
 *   0x28  TSS
 */

#ifndef OPENOS_ARCH_X86_GDT_H
#define OPENOS_ARCH_X86_GDT_H

#include <stdint.h>

/* Segment selectors */
#define GDT_KERNEL_CODE  0x08
#define GDT_KERNEL_DATA  0x10
#define GDT_USER_CODE    0x18
#define GDT_USER_DATA    0x20
#define GDT_TSS          0x28

/* Ring 3 selectors (RPL = 3) as loaded into segment registers */
#define USER_CODE_SELECTOR (GDT_USER_CODE | 3)  /* 0x1B */
#define USER_DATA_SELECTOR (GDT_USER_DATA | 3)  /* 0x23 */

/* GDT entry (segment descriptor) */
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

/* GDT pointer for lgdt */
struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/* 32-bit Task State Segment */
struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;      /* Kernel stack pointer loaded on ring 3 -> 0 */
    uint32_t ss0;       /* Kernel stack segment loaded on ring 3 -> 0 */
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

/* Install the kernel GDT and TSS. Call before enabling interrupts. */
void gdt_init(void);

/* Update the TSS kernel stack pointer (call on every context switch). */
void tss_set_kernel_stack(uint32_t esp0);

#endif /* OPENOS_ARCH_X86_GDT_H */
