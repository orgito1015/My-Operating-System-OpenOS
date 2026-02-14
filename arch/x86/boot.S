/*
 * OpenOS - Multiboot Kernel Loader
 * Boots via GRUB/QEMU in 32-bit protected mode and calls kmain()
 */

/* Multiboot header constants */
.set ALIGN,    1<<0             /* Align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* Provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* Multiboot flags */
.set MAGIC,    0x1BADB002       /* Multiboot magic number */
.set CHECKSUM, -(MAGIC + FLAGS) /* Checksum to verify multiboot header */

/* Multiboot header section */
.section .multiboot
    .align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

/* Kernel code section */
.section .text
    .global _start
    .extern kmain

_start:
    /* Disable interrupts during boot */
    cli

    /* Set up kernel stack (grows downward from stack_top) */
    mov $stack_top, %esp

    /* Call C kernel entry point */
    call kmain

    /* Kernel should never return, but if it does, halt */
.Lhalt:
    cli
    hlt
    jmp .Lhalt

/* Kernel stack section (uninitialized data) */
.section .bss
    .align 16
stack_bottom:
    .space 16384       /* 16 KiB stack */
stack_top:
