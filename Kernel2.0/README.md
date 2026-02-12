# OpenOS – Minimal Educational Kernel (Step 1: Keyboard Input)

This is the **first major feature** of **OpenOS**, a small educational
operating system kernel designed to help you learn how kernels work from
the ground up.

This step gives you:

- A **Multiboot-compatible** kernel entry (`boot.S`) for GRUB
- A simple **32‑bit protected mode** C `kmain()`
- **VGA text mode** output to the screen
- **Interrupt Descriptor Table (IDT)** for handling hardware interrupts
- **Programmable Interrupt Controller (PIC)** configuration
- **Keyboard driver** with PS/2 keyboard support
- **Interactive shell** that echoes user input
- A minimal **Makefile** to build and run the kernel with QEMU

The kernel boots, initializes interrupts, sets up the keyboard driver,
and provides an interactive prompt where users can type commands.

---

## Features

### Core Kernel
- Multiboot bootloader support
- 32-bit protected mode execution
- VGA text mode terminal with scrolling support

### Interrupt System
- **IDT (Interrupt Descriptor Table)**: 256 interrupt gate entries
- **PIC Remapping**: Master PIC (0x20-0x27), Slave PIC (0x28-0x2F)
- **IRQ Handling**: Proper interrupt service routines in assembly
- **EOI (End Of Interrupt)**: Correct acknowledgment to PIC

### Keyboard Driver
- **PS/2 Keyboard Support**: Scan code set 1
- **US QWERTY Layout**: Full ASCII translation table
- **Special Keys**: Shift, Caps Lock, Backspace, Enter
- **Modifier Key Support**: Proper shift and caps lock handling
- **Real-time Echo**: Characters appear as you type
- **Line Input**: Buffered input with Enter key processing

### Interactive Shell
- Command prompt: `OpenOS> `
- Echo typed input back to screen
- Ready for command parsing in future updates

---

## Prerequisites

You will need:

- A cross‑compiler (or native) toolchain that can target bare‑metal i386:
  - `i686-elf-gcc`
  - `i686-elf-binutils` (for `ld`, etc., if you prefer not to use GCC as the linker)
- An emulator, e.g.:
  - `qemu-system-i386`

On many Linux distros you can build your own `i686-elf-gcc` following
standard OSDev.org tutorials.

---

## Build

From the project root:

```bash
make
```

This produces:

- `openos.bin` – the kernel binary suitable for Multiboot (e.g. GRUB / QEMU)

---

## Run with QEMU (direct kernel loading)

```bash
make run
```

This is equivalent to:

```bash
qemu-system-i386 -kernel openos.bin
```

QEMU will boot directly into the kernel (as if loaded by a Multiboot
bootloader). You should see a text screen like:

```text
OpenOS - Educational Kernel Prototype
-------------------------------------
Running in 32-bit protected mode.
Initializing interrupts...
Keyboard initialized. Type something!

OpenOS> _
```

Now you can type on your keyboard and see the characters appear on screen!
Press Enter to submit your input, and the kernel will echo it back.

---

## How It Works

### Interrupt System

The kernel sets up a complete interrupt handling system:

1. **IDT Initialization**: Creates a table of 256 interrupt descriptors
2. **PIC Remapping**: Remaps hardware IRQs to avoid conflicts with CPU exceptions
3. **ISR Installation**: Installs assembly interrupt service routines
4. **Keyboard Handler**: IRQ1 (keyboard) triggers the keyboard interrupt handler

### Keyboard Driver

The keyboard driver works by:

1. **Interrupt Handling**: When a key is pressed, IRQ1 fires
2. **Scan Code Reading**: Driver reads scan code from port 0x60
3. **Translation**: Converts scan code to ASCII using lookup table
4. **Modifier Keys**: Tracks shift and caps lock state
5. **Character Echo**: Sends character to VGA terminal
6. **Line Buffering**: Stores input until Enter is pressed
7. **EOI Signal**: Acknowledges interrupt to PIC

### Special Keys

- **Shift**: Modifies character output (uppercase, symbols)
- **Caps Lock**: Toggles case for letters
- **Backspace**: Removes last character from screen and buffer
- **Enter**: Completes line input and processes command

---

## Testing Keyboard Input

1. Build and run: `make run`
2. Type letters, numbers, and symbols
3. Test Shift key for uppercase and special characters
4. Test Caps Lock toggle
5. Test Backspace to delete characters
6. Press Enter to submit input
7. Verify the kernel echoes your input

---

## File Structure

### New Files

- `idt.h` / `idt.c` - Interrupt Descriptor Table implementation
- `pic.h` / `pic.c` - Programmable Interrupt Controller setup
- `isr.h` / `isr.S` - Interrupt Service Routines (assembly)
- `keyboard.h` / `keyboard.c` - Keyboard driver

### Modified Files

- `kernel.c` - Updated with interrupt initialization and interactive loop
- `Makefile` - Updated to build new source files

### Original Files

- `boot.S` - Multiboot bootloader
- `linker.ld` - Linker script

---

## Next Steps (Roadmap)

Future steps for OpenOS (that we can implement incrementally):

1. **CPU Exception Handlers**
   - Page fault handler
   - General protection fault handler
   - Other CPU exceptions

2. **Timer & Scheduling**
   - PIT (timer) initialization
   - A global tick counter
   - Basic preemptive multitasking

3. **Memory Management**
   - Physical memory bitmap allocator
   - Basic paging (identity map + simple higher-half kernel)

4. **Processes & Scheduler**
   - `struct process` with register state
   - Context switch routine
   - Round‑robin scheduler driven by the timer interrupt

5. **Shell Commands**
   - Parse user input
   - Implement basic commands (help, clear, echo, etc.)
   - Command history

6. **Syscalls & User Space**
   - Simple syscall interface
   - Minimal userland program loader

Each of these can be added in small, understandable commits on top of this
kernel.

---

## License

You can license this however you like in your own repo. For now, consider
this initial scaffold MIT‑style: do whatever you want with it, with no
warranty of any kind.
