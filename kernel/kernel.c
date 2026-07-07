/*
 * OpenOS - Kernel Main Entry Point
 * 
 * This is the main entry point for the OpenOS kernel after boot.
 * It initializes all kernel subsystems and enters the main kernel loop.
 */

#include "kernel.h"
#include "shell.h"
#include "../arch/x86/idt.h"
#include "../arch/x86/pic.h"
#include "../arch/x86/isr.h"
#include "../arch/x86/exceptions.h"
#include "../drivers/keyboard.h"
#include "../drivers/timer.h"
#include "../drivers/console.h"
#include "../fs/vfs.h"
#include "../include/ipc.h"
#include "../include/smp.h"
#include "../include/gui.h"
#include "../include/network.h"
#include "../include/script.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "../memory/heap.h"

/* Current working directory */
vfs_node_t* current_directory = 0;

/* Get current directory */
vfs_node_t* kernel_get_current_directory(void) {
    return current_directory;
}

/* Set current directory */
void kernel_set_current_directory(vfs_node_t* dir) {
    if (dir && dir->type == NODE_DIRECTORY) {
        current_directory = dir;
    }
}

/* Kernel entry point called from boot.S */
void kmain(struct multiboot_info *mboot) {
    /* Initialize console */
    console_init();
    
    console_write("OpenOS - Advanced Educational Kernel\n");
    console_write("====================================\n");
    console_write("Running in 32-bit protected mode.\n\n");

    /* Initialize IDT */
    console_write("[1/13] Initializing IDT...\n");
    idt_init();
    
    /* Install exception handlers */
    console_write("[2/13] Installing exception handlers...\n");
    exceptions_init();
    
    /* Initialize PIC */
    console_write("[3/13] Initializing PIC...\n");
    pic_init();
    
    /* Initialize timer (100 Hz) */
    console_write("[4/13] Initializing timer...\n");
    timer_init(100);
    
    /* Install timer interrupt handler (IRQ0 = interrupt 0x20) */
    idt_set_gate(0x20, (uint32_t)irq0_handler, KERNEL_CODE_SEGMENT, IDT_FLAGS_KERNEL);
    
    /* Install keyboard interrupt handler (IRQ1 = interrupt 0x21) */
    console_write("[5/13] Initializing keyboard...\n");
    idt_set_gate(0x21, (uint32_t)irq1_handler, KERNEL_CODE_SEGMENT, IDT_FLAGS_KERNEL);
    
    /* Initialize keyboard (this will unmask IRQ1) */
    keyboard_init();
    
    /* Initialize VFS */
    console_write("[6/13] Initializing filesystem...\n");
    vfs_init();
    current_directory = vfs_get_root();
    
    /* Initialize physical memory manager */
    console_write("[7/13] Initializing physical memory manager...\n");
    pmm_init(mboot);

    /* Initialize virtual memory manager */
    console_write("[8/13] Initializing virtual memory manager...\n");
    vmm_init();

    /*
     * Initialize the kernel heap on a PMM-backed region.
     *
     * Reserve a contiguous run of physical pages while paging still maps
     * all RAM identity (virt == phys), so the heap base is valid as a
     * pointer. 1024 pages = 4 MiB of dynamic kernel memory.
     */
    {
        #define HEAP_PAGES 1024u
        void *heap_start = pmm_alloc_page();
        void *heap_prev  = heap_start;
        uint32_t got = (heap_start != 0) ? 1u : 0u;
        while (got < HEAP_PAGES) {
            void *p = pmm_alloc_page();
            /* Require physically contiguous pages for a flat arena. */
            if (p == 0 || (uint32_t)p != (uint32_t)heap_prev + 0x1000u) {
                if (p != 0) {
                    pmm_free_page(p);
                }
                break;
            }
            heap_prev = p;
            got++;
        }
        if (heap_start != 0 && got > 0u) {
            heap_init(heap_start, (size_t)got * 0x1000u);
        }
        #undef HEAP_PAGES
    }

    /* Initialize IPC mechanisms */
    console_write("[9/13] Initializing IPC (pipes, message queues)...\n");
    ipc_init();
    
    /* Initialize SMP support */
    console_write("[10/13] Initializing multi-core SMP...\n");
    smp_init();
    
    /* Initialize GUI/Windowing system */
    console_write("[11/13] Initializing GUI framework...\n");
    gui_init();
    
    /* Initialize Networking stack */
    console_write("[12/13] Initializing networking stack...\n");
    net_init();
    
    /* Initialize Shell scripting */
    console_write("[13/13] Initializing shell scripting...\n");
    script_init();
    
    /* Enable interrupts */
    __asm__ __volatile__("sti");
    
    /* Now that interrupts are enabled, unmask the timer IRQ */
    pic_unmask_irq(0);
    
    console_write("\n*** System Ready ***\n");
    console_write("- Exception handling: Active\n");
    console_write("- Timer interrupts: 100 Hz\n");
    console_write("- Keyboard: Ready\n");
    console_write("- Filesystem: Ready\n");
    console_write("- IPC: Pipes and message queues available\n");
    console_write("- SMP: Multi-core support enabled\n");
    console_write("- GUI: Windowing system ready\n");
    console_write("- Network: TCP/IP stack initialized\n");
    console_write("- Scripting: Shell scripting enabled\n\n");
    console_write("Type 'help' for available commands.\n\n");
    
    /* Initialize shell */
    shell_init();
    
    /* Interactive shell loop */
    char input[256];
    while (1) {
        console_write("OpenOS> ");
        keyboard_get_line(input, sizeof(input));
        shell_execute(input);
    }
}
