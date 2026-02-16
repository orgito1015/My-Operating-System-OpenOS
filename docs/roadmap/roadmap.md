# OpenOS Development Roadmap

## Current Status (v0.1)

✅ **Completed Features:**
- Multiboot-compliant bootloader
- 32-bit protected mode kernel
- VGA text mode terminal with scrolling
- Interrupt Descriptor Table (IDT) setup
- Programmable Interrupt Controller (PIC) configuration
- PS/2 keyboard driver with modifier key support
- Interactive shell prompt
- Basic I/O port operations

## Phase 1: Core Interrupt System (Current)

**Goal**: Robust interrupt handling and basic I/O

### Completed
- [x] IDT initialization
- [x] PIC remapping
- [x] Keyboard interrupt handler (IRQ1)
- [x] Scan code to ASCII translation
- [x] Shift and Caps Lock support
- [x] Backspace and Enter handling
- [x] Line buffering

### Remaining
- [ ] Exception handlers (page fault, GPF, divide by zero, etc.)
- [ ] Better error reporting for exceptions
- [ ] Double fault handler

## Phase 2: Timer and Basic Scheduling

**Goal**: Add time awareness and prepare for multitasking

### Tasks
- [ ] PIT (Programmable Interval Timer) initialization
- [ ] Timer interrupt handler (IRQ0)
- [ ] Global tick counter
- [ ] Basic delay/sleep functions
- [ ] Uptime tracking

### Estimated Complexity
- **Difficulty**: Easy-Medium
- **Files to modify**: `kernel.c`, new `timer.c/h`
- **Dependencies**: PIC and IDT (already done)

## Phase 3: Memory Management

**Goal**: Proper memory allocation and virtual memory

### Physical Memory Manager
- [ ] Memory map from bootloader (Multiboot info)
- [ ] Bitmap-based physical page allocator
- [ ] Page frame allocation/deallocation
- [ ] Memory statistics (total, used, free)

### Virtual Memory (Paging)
- [ ] Page directory and page table structures
- [ ] Identity mapping for kernel
- [ ] Higher-half kernel mapping (optional)
- [ ] Page fault handler
- [ ] Virtual address allocation

### Kernel Heap
- [ ] Simple heap allocator (malloc/free)
- [ ] First-fit or best-fit algorithm
- [ ] Heap expansion/contraction
- [ ] Memory leak detection (debug mode)

### Estimated Complexity
- **Difficulty**: Medium-Hard
- **Files**: `memory.c/h`, `paging.c/h`, `heap.c/h`
- **Critical**: Page fault handler is essential

## Phase 4: Process Management

**Goal**: Multiple processes and context switching

### Process Abstraction
- [ ] `struct process` with register state
- [ ] Process ID allocation
- [ ] Process state (ready, running, blocked)
- [ ] Process creation/termination
- [ ] Process list/queue

### Context Switching
- [ ] Save/restore CPU registers
- [ ] Switch page directories
- [ ] Switch stack pointers
- [ ] TSS (Task State Segment) setup

### Simple Scheduler
- [ ] Round-robin scheduling algorithm
- [ ] Priority levels (optional)
- [ ] Scheduler tick (integrate with timer)
- [ ] Idle process

### Estimated Complexity
- **Difficulty**: Hard
- **Files**: `process.c/h`, `scheduler.c/h`, `context.S`
- **Dependencies**: Timer, Memory Management

## Phase 5: System Calls

**Goal**: User/kernel mode separation and syscall interface

### User Mode Transition
- [ ] User mode page tables
- [ ] Ring 3 segments in GDT
- [ ] Stack switching (kernel/user stacks)
- [ ] Privilege level changes

### Syscall Interface
- [ ] `int 0x80` or `sysenter` mechanism
- [ ] Syscall handler
- [ ] Syscall table/dispatcher
- [ ] Return value handling

### Initial Syscalls
- [ ] `exit()` - Process termination
- [ ] `write()` - Output to terminal
- [ ] `read()` - Keyboard input
- [ ] `fork()` - Process creation (advanced)
- [ ] `exec()` - Load program (advanced)

### Estimated Complexity
- **Difficulty**: Medium-Hard
- **Files**: `syscall.c/h`, `syscall.S`, modifications to `process.c`
- **Dependencies**: Process Management

## Phase 6: Enhanced Shell

**Goal**: Useful command-line interface

### Command Parsing
- [ ] Tokenize input (split by spaces)
- [ ] Command registry/lookup
- [ ] Argument parsing

### Built-in Commands
- [ ] `help` - Show available commands
- [ ] `clear` - Clear screen
- [ ] `echo` - Print arguments
- [ ] `uptime` - Show system uptime
- [ ] `meminfo` - Display memory statistics
- [ ] `ps` - List processes
- [ ] `kill` - Terminate process
- [ ] `reboot` - Restart system

### Command History
- [ ] Store previous commands
- [ ] Up/down arrow navigation
- [ ] History buffer (circular)

### Estimated Complexity
- **Difficulty**: Easy-Medium
- **Files**: `shell.c/h`, `commands.c/h`

## Phase 7: File System (Future)

**Goal**: Persistent storage and file I/O

### Disk Driver
- [ ] ATA/IDE PIO mode driver
- [ ] Read/write sectors
- [ ] Disk detection

### File System
- [ ] Simple file system (custom or FAT12/16)
- [ ] File operations (open, close, read, write)
- [ ] Directory operations
- [ ] Virtual file system (VFS) layer

### Estimated Complexity
- **Difficulty**: Very Hard
- **Files**: `ata.c/h`, `fs.c/h`, `vfs.c/h`

## Phase 8: Advanced Features (Long-term)

### Networking
- [ ] NE2000 or RTL8139 network driver
- [ ] TCP/IP stack (simplified)
- [ ] Sockets interface

### Graphics
- [ ] VESA VBE framebuffer
- [ ] Basic windowing system
- [ ] Mouse support

### Advanced IPC
- [ ] Pipes
- [ ] Shared memory
- [ ] Message queues

### Multi-core Support
- [ ] SMP (Symmetric Multiprocessing)
- [ ] Per-CPU data structures
- [ ] Spinlocks and synchronization

## Contributing to the Roadmap

Want to implement a feature? Great!

1. **Pick a task** from the current or next phase
2. **Discuss your approach** (open an issue)
3. **Implement incrementally** (small PRs are better)
4. **Test thoroughly** (especially with QEMU)
5. **Document your changes** (code comments + docs)

### Difficulty Guide

- **Easy**: Basic C knowledge, clear instructions
- **Medium**: Understanding of OS concepts required
- **Hard**: Deep understanding of x86 architecture
- **Very Hard**: Extensive research and debugging needed

## Learning Resources

For each phase, these resources will help:

### General
- [OSDev Wiki](https://wiki.osdev.org/) - Start here
- [The little book about OS development](https://littleosbook.github.io/)
- [xv6 Source Code](https://github.com/mit-pdos/xv6-public) - Great reference

### Phase-Specific
- **Interrupts**: [OSDev Interrupts](https://wiki.osdev.org/Interrupts)
- **Memory**: [OSDev Paging](https://wiki.osdev.org/Paging)
- **Processes**: [OSDev Multitasking](https://wiki.osdev.org/Multitasking)
- **Syscalls**: [OSDev System Calls](https://wiki.osdev.org/System_Calls)

## Version History

- **v0.1 (Current)**: Basic interrupt handling and keyboard I/O
- **v0.2 (Planned)**: Exception handlers and timer
- **v0.3 (Planned)**: Memory management
- **v0.4 (Planned)**: Process management and scheduling
- **v1.0 (Goal)**: Full multitasking OS with syscalls and shell

---

**Last Updated**: February 2026

This roadmap is a living document and will evolve as the project grows. Suggestions and contributions are always welcome!
