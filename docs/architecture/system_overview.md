# OpenOS Architecture

## Overview

OpenOS is a 32-bit x86 educational operating system kernel designed to demonstrate fundamental OS concepts. It runs in protected mode and provides a minimal but functional interrupt-driven system with keyboard support.

## System Architecture

### Boot Process

1. **GRUB/Multiboot Loader**
   - The system boots using a Multiboot-compliant bootloader (GRUB or QEMU direct kernel loading)
   - The Multiboot header is located in `boot.S`

2. **Entry Point (`_start` in boot.S)**
   - Sets up a 16KB kernel stack
   - Calls the C kernel entry point `kmain()`

3. **Kernel Initialization (`kmain` in kernel.c)**
   - Initializes VGA text mode terminal
   - Sets up Interrupt Descriptor Table (IDT)
   - Configures Programmable Interrupt Controller (PIC)
   - Installs keyboard interrupt handler
   - Enables hardware interrupts
   - Enters interactive shell loop

### Memory Layout

```
Physical Memory Map:
┌──────────────────────┬─────────────┐
│ 0x00000000           │ BIOS/IVT    │
├──────────────────────┼─────────────┤
│ 0x00007C00           │ Bootloader  │
├──────────────────────┼─────────────┤
│ 0x00100000 (1 MiB)   │ Kernel      │
│  - .multiboot        │ Multiboot   │
│  - .text             │ Code        │
│  - .rodata           │ Read-only   │
│  - .data             │ Data        │
│  - .bss              │ BSS         │
│  - Stack (16KB)      │ Stack       │
└──────────────────────┴─────────────┘
```

### Segmentation

OpenOS uses a flat memory model with the following GDT segments:

- **0x08**: Kernel Code Segment (32-bit, executable, readable)
- **0x10**: Kernel Data Segment (32-bit, writable, readable)

### Interrupt Handling

#### Interrupt Descriptor Table (IDT)

- **Size**: 256 entries (standard x86 IDT)
- **Location**: Defined in `idt.c`
- **Structure**: Each entry is 8 bytes containing:
  - Handler address (split into low/high 16 bits)
  - Segment selector (0x08 for kernel code)
  - Type and attributes (0x8E = present, DPL=0, 32-bit interrupt gate)

#### Programmable Interrupt Controller (PIC)

The 8259A PIC is configured as follows:

- **Master PIC**: IRQ 0-7 remapped to interrupts 0x20-0x27
- **Slave PIC**: IRQ 8-15 remapped to interrupts 0x28-0x2F

This remapping avoids conflicts with CPU exceptions (0x00-0x1F).

**IRQ Assignments:**
- IRQ0 (0x20): Timer (not yet implemented)
- IRQ1 (0x21): Keyboard (implemented)
- IRQ2: Cascade (slave PIC connection)
- IRQ3-7: Other devices (not implemented)
- IRQ8-15: Slave PIC devices (not implemented)

#### Interrupt Flow

```
Hardware Event (e.g., key press)
    ↓
PIC receives signal on IRQ1
    ↓
PIC sends interrupt to CPU (vector 0x21)
    ↓
CPU looks up handler in IDT[0x21]
    ↓
CPU pushes state and jumps to irq1_handler (isr.S)
    ↓
irq1_handler saves registers
    ↓
irq1_handler calls keyboard_handler() in C
    ↓
keyboard_handler reads scancode, processes key
    ↓
keyboard_handler sends EOI to PIC
    ↓
irq1_handler restores registers
    ↓
iret instruction returns to interrupted code
```

## Component Details

### VGA Text Mode Terminal

- **Buffer Address**: 0xB8000 (VGA text mode memory)
- **Dimensions**: 80 columns × 25 rows
- **Format**: Each character is 2 bytes (character + color attribute)
- **Features**:
  - Character output
  - Newline handling
  - Scrolling when screen is full
  - Backspace support

### Keyboard Driver

**Components:**
- `keyboard.h/c` - PS/2 keyboard driver implementation
- `isr.S` - Assembly interrupt service routine

**Features:**
- Scan code set 1 (US QWERTY layout)
- Scan code to ASCII translation
- Modifier key support (Shift, Caps Lock)
- Special key handling (Enter, Backspace)
- Line buffering (waits for Enter key)
- Real-time echo to terminal

**State Tracking:**
- `shift_pressed` - Current shift key state
- `caps_lock` - Caps lock toggle state
- `input_buffer` - 256-byte line buffer
- `line_ready` - Flag indicating complete line

### Interactive Shell

A simple command prompt that:
1. Displays "OpenOS> " prompt
2. Accepts keyboard input
3. Echoes back the typed line
4. Ready for command parsing (future enhancement)

## Current Limitations

- **No Memory Management**: No paging, no heap allocator
- **No Process Management**: Single-threaded, no task switching
- **No Exception Handlers**: CPU exceptions are not caught
- **No Timer**: No periodic interrupts or time tracking
- **No Filesystem**: No disk access
- **No User Mode**: Everything runs at privilege level 0

## Future Architecture Plans

See [roadmap.md](roadmap.md) for planned enhancements:

1. Exception handlers for page faults, GPF, etc.
2. Timer interrupt and scheduling
3. Physical memory management
4. Virtual memory with paging
5. Process abstraction and multitasking
6. System calls for user-mode programs
7. Basic filesystem support

## Building Blocks

### Core Files

| File | Purpose |
|------|---------|
| `boot.S` | Multiboot header and entry point |
| `kernel.c` | Main kernel logic and VGA terminal |
| `idt.c/h` | Interrupt descriptor table management |
| `pic.c/h` | PIC initialization and EOI handling |
| `isr.S/h` | Assembly interrupt service routines |
| `keyboard.c/h` | Keyboard driver implementation |
| `linker.ld` | Linker script for memory layout |

### Compilation

The kernel is compiled with:
- `-ffreestanding` - No hosted environment
- `-nostdlib` - No standard library
- `-m32` - 32-bit x86 target
- `-O2` - Optimization level 2
- `-Wall -Wextra` - All warnings enabled

## References

- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development resource
- [Intel® 64 and IA-32 Architectures Software Developer's Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- [8259 PIC Documentation](https://wiki.osdev.org/8259_PIC)
