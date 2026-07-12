/*
 * OpenOS - Global Descriptor Table (GDT) and Task State Segment (TSS)
 *
 * See gdt.h for the selector layout. The GDT is installed early in
 * boot, replacing whatever descriptor table the bootloader left behind,
 * so the kernel controls its own segmentation and can run ring 3 code.
 */

#include "gdt.h"
#include "../../kernel/string.h"

#define GDT_ENTRIES 6

static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_ptr   gdtp;
static struct tss_entry tss;

/* Assembly helpers (arch/x86/context.S) */
extern void gdt_flush(uint32_t gdtp_addr);
extern void tss_flush(void);

/* Encode one descriptor */
static void gdt_set_gate(int num, uint32_t base, uint32_t limit,
                         uint8_t access, uint8_t gran) {
    gdt[num].base_low    = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = limit & 0xFFFF;
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void gdt_init(void) {
    gdtp.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gdtp.base  = (uint32_t)&gdt;

    /* 0x00: null descriptor */
    gdt_set_gate(0, 0, 0, 0, 0);

    /* 0x08: kernel code - base 0, limit 4 GiB, ring 0, exec/read */
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xCF);

    /* 0x10: kernel data - base 0, limit 4 GiB, ring 0, read/write */
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xCF);

    /* 0x18: user code - base 0, limit 4 GiB, ring 3, exec/read */
    gdt_set_gate(3, 0, 0xFFFFF, 0xFA, 0xCF);

    /* 0x20: user data - base 0, limit 4 GiB, ring 3, read/write */
    gdt_set_gate(4, 0, 0xFFFFF, 0xF2, 0xCF);

    /* 0x28: TSS descriptor (access 0x89 = present, ring 0, 32-bit TSS) */
    {
        uint32_t base  = (uint32_t)&tss;
        uint32_t limit = sizeof(tss) - 1;

        /* Zero the TSS */
        uint8_t *p = (uint8_t *)&tss;
        for (uint32_t i = 0; i < sizeof(tss); i++) {
            p[i] = 0;
        }

        tss.ss0  = GDT_KERNEL_DATA;
        tss.esp0 = 0;   /* Set per-process by the scheduler */
        /* No I/O permission bitmap: base beyond the segment limit */
        tss.iomap_base = sizeof(tss);

        gdt_set_gate(5, base, limit, 0x89, 0x00);
    }

    /* Load the GDT (reloads CS via far jump and data segments) and TSS */
    gdt_flush((uint32_t)&gdtp);
    tss_flush();
}

void tss_set_kernel_stack(uint32_t esp0) {
    tss.esp0 = esp0;
}
