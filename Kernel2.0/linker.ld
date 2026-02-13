/*
 * OpenOS Linker Script
 * Defines the memory layout and section arrangement for the kernel
 */

/* Entry point symbol from boot.S */
ENTRY(_start)

SECTIONS
{
  /* Load the kernel at 1 MiB (0x100000)
   * This is a standard location above conventional memory
   * and below the 4 GiB boundary for 32-bit addressing
   */
  . = 1M;

  /* Multiboot header must be in the first 8 KiB of the kernel
   * This section contains the Multiboot magic, flags, and checksum
   */
  .multiboot :
  {
    *(.multiboot)
  }

  /* Code section - contains all executable instructions
   * Aligned to 4 KiB page boundary for future paging support
   */
  .text ALIGN(4K) :
  {
    *(.text*)
  }

  /* Read-only data section - contains string literals and const data
   * Aligned to 4 KiB for memory protection (future)
   */
  .rodata ALIGN(4K) :
  {
    *(.rodata*)
  }

  /* Initialized data section - contains global/static variables
   * with initial values
   */
  .data ALIGN(4K) :
  {
    *(.data*)
  }

  /* Uninitialized data section - contains global/static variables
   * without initial values (zeroed by bootloader)
   * COMMON is for uninitialized C data
   */
  .bss ALIGN(4K) :
  {
    *(.bss*)
    *(COMMON)
  }

  /* Discard unnecessary sections */
  /DISCARD/ :
  {
    *(.note.gnu.build-id)
    *(.comment)
    *(.eh_frame)
  }
}
