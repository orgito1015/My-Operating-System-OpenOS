# OpenOS - Advanced Research Kernel

OpenOS is a research-oriented, open-source operating system built from scratch for the x86 architecture. It features a **modular monolithic architecture**, complete exception handling, memory management, and timer support - a production-ready foundation for OS research and experimentation.

The goal is to build a small, understandable OS **from zero**, inspired by hobby OS projects like MyraOS, xv6, and OSDev examples — but implemented with **our own code**, fully documented, and open for community contribution.

##  Mission
To create a collaborative OS development environment where researchers, academics, and low-level system developers can explore:

- How CPUs boot an OS  
- What a kernel actually does  
- How memory, interrupts, and drivers work  
- How processes and syscalls operate  
- How filesystems and user programs work  

All with clean, simple, modern C + Assembly code in a **well-organized modular structure**.

##  Architecture

OpenOS follows a **modular monolithic kernel architecture**:

```
├── arch/x86/          # Architecture-specific code (IDT, ISR, PIC, exceptions)
├── kernel/            # Core kernel (initialization, panic handling)
├── kernel/cpu/        # CPU simulation (pipeline, single-cycle, performance)
├── memory/            # Memory management (PMM, VMM, heap, cache, bus)
├── drivers/           # Hardware drivers (console, keyboard, timer)
├── fs/                # File systems (VFS - future)
├── process/           # Process management (future)
├── benchmarks/        # Performance benchmarks and test programs
└── include/           # Common headers (types, multiboot)
```

**Benefits**:
- ✅ Clear separation of concerns
- ✅ No circular dependencies
- ✅ Easy to navigate and understand
- ✅ Scalable for future growth
- ✅ Industry-standard organization

See [docs/architecture/ARCHITECTURE.md](docs/architecture/ARCHITECTURE.md) for detailed architecture documentation.

##  Features

### Phase 0 - Core Foundation (✅ Complete)
- ✅ **Modular monolithic architecture** - Clean separation of concerns
- ✅ **Multiboot-compatible 32-bit kernel** - GRUB bootloader support
- ✅ **VGA text output** - 80x25 color text mode via console driver
- ✅ **Complete exception handling** - All 32 x86 CPU exceptions with detailed crash reports
- ✅ **Interrupt handling** - IDT, ISRs, IRQs with PIC management
- ✅ **Physical memory manager (PMM)** - Bitmap-based page frame allocator
- ✅ **Virtual memory manager (VMM)** - Two-level paging, page tables, TLB management
- ✅ **Timer driver (PIT)** - Programmable Interval Timer at 100 Hz
- ✅ **Keyboard driver** - PS/2 keyboard with line buffering
- ✅ **VirtualBox automation** - Automated VM creation and ISO deployment
- ✅ **CPU Architecture Simulator** - 5-stage pipeline, single-cycle CPU, cache, and bus simulation

### Phase 0.5 - CPU Simulation Framework (✅ Complete)
- ✅ **5-Stage Pipelined CPU** - IF/ID/EX/MEM/WB with hazard detection
- ✅ **Single-Cycle CPU** - Reference implementation for comparison
- ✅ **Direct-Mapped Cache** - 256 lines, 32-byte blocks, hit/miss tracking
- ✅ **Memory Bus Simulator** - 64-bit width, 800 MHz, latency modeling
- ✅ **Performance Counters** - CPI, MIPS, cache statistics
- ✅ **Benchmark Suite** - Compare pipeline vs single-cycle performance

### Phase 0.6 - Coreutils-style Shell Commands (✅ Complete)
- ✅ **`ls`** - List directory contents with flags: `-a` (show dotfiles), `-l` (long format), `-R` (recursive), `-h` (help)
- ✅ **`cat`** - Display file contents with flags: `-n` (line numbers), `-h` (help), multiple file support
- ✅ **`stat`** - Show file/directory metadata: name, type, size, inode number

### Phase 1 - Process Management (Planned)
- 🔲 Process structures and state management
- 🔲 Context switching between processes
- 🔲 Round-robin scheduler
- 🔲 fork() system call
- 🔲 Basic process management

### Future Phases ( Roadmap)
- Kernel heap allocator
- System calls and user mode
- Shell and userland programs
- Simple filesystem
- Advanced scheduling
- And more! See [roadmap.md](docs/roadmap/roadmap.md)

##  Quick Start

### Build and Run (QEMU)

```bash
make run
```

You should see:

```
OpenOS - Advanced Research Kernel
====================================
Running in 32-bit protected mode.

[1/5] Initializing IDT...
[2/5] Installing exception handlers...
[3/5] Initializing PIC...
[4/5] Initializing timer...
[5/5] Initializing keyboard...

*** System Ready ***
- Exception handling: Active
- Timer interrupts: 100 Hz
- Keyboard: Ready

Type commands and press Enter!

OpenOS> _
```

### Build Bootable ISO

```bash
make iso
```

### Run in VirtualBox

```bash
make run-vbox
```

This automatically creates a VM, builds an ISO, and launches OpenOS in VirtualBox!

### Run CPU Architecture Benchmark

```bash
make benchmark
```

This runs a comprehensive benchmark comparing the 5-stage pipelined CPU against the single-cycle CPU model:

```
OpenOS CPU Architecture Simulator
==================================

=== Pipelined CPU Benchmark ===
Instructions executed: 8192
Total cycles: 10245
Pipeline stalls: 2048
CPI: 1.251
MIPS: 799.61

=== Single-Cycle CPU Benchmark ===
Instructions executed: 8192
Total cycles: 8192
CPI: 1.000
MIPS: 1000.00

=== Cache Performance Benchmark ===
Cache hits: 3316, Cache misses: 6684
Hit rate: 33.16%

=== Memory Bus Performance ===
Bus frequency: 800 MHz, Memory latency: 24 cycles (30.0 ns)
Throughput: 6103.52 MB/s
```

See [kernel/cpu/README.md](kernel/cpu/README.md) for detailed documentation on the CPU simulator.

## 🛠️ Build & Run

### Prerequisites
To build and run OpenOS, you'll need:

- **gcc** with 32-bit support (or i686-elf-gcc cross-compiler)
- **nasm** (if you plan to extend the assembly code)
- **make**
- **qemu-system-i386** (for testing with QEMU)
- **grub-pc-bin**, **xorriso**, **mtools** (for creating bootable ISO)
- **VirtualBox** (optional, for running in VirtualBox)

#### Installing Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt-get install gcc-multilib nasm make qemu-system-x86 grub-pc-bin xorriso mtools
```

**Arch Linux:**
```bash
sudo pacman -S gcc nasm make qemu-system-x86 grub xorriso mtools
```

**macOS (with Homebrew):**
```bash
brew install i686-elf-gcc nasm make qemu grub xorriso mtools
```

### Building

From the project root directory:

```bash
make
```

This will compile the kernel and produce `Kernel2.0/openos.bin`.

### Running

#### Option 1: QEMU (Quick Testing)

To build and run the kernel in QEMU with direct kernel boot:

```bash
make run
```

This launches QEMU with the kernel. You should see:

```
OpenOS - Research Kernel Prototype
-------------------------------------
Running in 32-bit protected mode.
Initializing interrupts...
Keyboard initialized. Type something!

OpenOS> _
```

Type on your keyboard and press Enter to interact with the shell!

## Shell Commands

OpenOS includes a built-in shell with the following commands:

| Command | Description |
|---------|-------------|
| `help` | Display list of available commands |
| `clear` | Clear the console screen |
| `echo [text]` | Print text to the console |
| `uname` | Display OS name and version |
| `uptime` | Show system uptime |
| `pwd` | Print current working directory |
| `ls [-a] [-l] [-R] [-h] [path]` | List directory contents |
| `cd <path>` | Change current directory |
| `cat [-n] [-h] file [file...]` | Display file contents |
| `stat [-h] path` | Show file or directory metadata |
| `reboot` | Reboot the system |

### `ls` — List Directory Contents

```
ls [-a] [-l] [-R] [-h] [path]
```

| Flag | Description |
|------|-------------|
| `-a` | Include entries starting with `.` (dotfiles) |
| `-l` | Long format: shows type (`d`/`-`), size in bytes, and name |
| `-R` | Recursively list subdirectories |
| `-h` | Show help |

**Examples:**

```
OpenOS> ls /
bin/  etc/  home/  tmp/

OpenOS> ls -l /etc
-        59  motd.txt

OpenOS> ls -R /
/:
bin/  etc/  home/  tmp/
/bin:
/etc:
motd.txt
/home:
/tmp:
```

### `cat` — Display File Contents

```
cat [-n] [-h] file [file...]
```

| Flag | Description |
|------|-------------|
| `-n` | Number all output lines |
| `-h` | Show help |

**Examples:**

```
OpenOS> cat /etc/motd.txt
Welcome to OpenOS!
This is a test file in the filesystem.

OpenOS> cat -n /etc/motd.txt
     1  Welcome to OpenOS!
     2  This is a test file in the filesystem.
```

### `stat` — Show File Metadata

```
stat [-h] path
```

**Example:**

```
OpenOS> stat /etc/motd.txt
  File: motd.txt
  Type: regular file
  Size: 59 bytes
 Inode: 6

OpenOS> stat /etc
  File: etc
  Type: directory
  Size: 0 bytes
 Inode: 2
```

##  Phase 0 Implementation Details

OpenOS Phase 0 includes complete implementations of:

- **Exception Handling** - All 32 x86 exceptions with detailed crash reports showing registers, error codes, and faulting addresses
- **Physical Memory Manager** - Bitmap-based allocator supporting up to 4GB RAM
- **Virtual Memory Manager** - Complete two-level paging with page tables, TLB management, and region mapping
- **Timer Driver** - PIT configured at 100 Hz for future scheduling
- **Enhanced Kernel** - Clean boot messages, progress indicators, and modular design

For complete implementation details, see [docs/roadmap/UPGRADE_PHASE0.md](docs/roadmap/UPGRADE_PHASE0.md).

##  Testing Exception Handling

Want to see the exception handler in action? Add this to kernel.c:

```c
void test_exception(void) {
    volatile int x = 1 / 0;  // Trigger divide-by-zero
}
```

You'll get a detailed crash report with full register dump!

#### Option 2: VirtualBox (Full Emulation)

To create a bootable ISO and run in VirtualBox:

```bash
make run-vbox
```

This will:
1. Build the kernel
2. Create a bootable ISO image with GRUB (`openos.iso`)
3. Create and configure a VirtualBox VM
4. Start the VM with the ISO attached

**Manual VirtualBox Setup:**

If you prefer to set up VirtualBox manually:

1. Create the ISO image:
   ```bash
   make iso
   ```

2. Create a new VirtualBox VM:
   - Name: OpenOS
   - Type: Other
   - Version: Other/Unknown
   - Memory: 512 MB (minimum)
   - No hard disk needed

3. Configure the VM:
   - System → Boot Order: CD-ROM first
   - System → Enable I/O APIC
   - Storage → Add IDE Controller
   - Storage → Attach `openos.iso` as CD-ROM

4. Start the VM and enjoy!

**Troubleshooting:** If you encounter any issues with VirtualBox, see the [VirtualBox Troubleshooting Guide](docs/virtualization/VIRTUALBOX_TROUBLESHOOTING.md).

#### Option 3: ISO in QEMU

To test the ISO image in QEMU:

```bash
make run-iso
```

### Debugging with QEMU + GDB

OpenOS ships with a `make debug` target that starts QEMU paused with a GDB
server enabled so you can attach a debugger before any kernel code runs.

**Terminal 1 – start QEMU in debug mode:**

```bash
make debug
```

**Terminal 2 – attach GDB:**

```bash
make gdb
```

Then in GDB:

```
(gdb) continue       # resume kernel execution
(gdb) Ctrl+C         # interrupt to inspect state
(gdb) info registers # view CPU registers
```

For a full reference including QEMU logging (`make qemu-log`) and useful GDB
commands, see [docs/debugging.md](docs/debugging.md).

### Cleaning

To remove build artifacts:

```bash
make clean
```

##  Contributing

OpenOS welcomes contributions at all levels! Whether you're fixing a bug, adding a feature, improving documentation, or just learning, your contributions are valuable.

Please see [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines on:
- How to submit contributions
- Coding standards and style guidelines
- Testing procedures
- Areas where help is needed

### Quick Start for Contributors

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes and test them
4. Commit with clear messages (`git commit -m "Add amazing feature"`)
5. Push to your fork (`git push origin feature/amazing-feature`)
6. Open a Pull Request

## Documentation

The `/docs` directory contains comprehensive documentation organized by topic:

- **[Documentation Index](docs/README.md)** - Complete documentation guide and structure
- **Architecture**: System design and implementation details
  - [Architecture Overview](docs/architecture/ARCHITECTURE.md) - Modular monolithic architecture
  - [System Overview](docs/architecture/system_overview.md) - Boot process and memory layout
- **Roadmap**: Development plans and milestones
  - [Roadmap](docs/roadmap/roadmap.md) - Future development plans
  - [Phase 0 Completion](docs/roadmap/UPGRADE_PHASE0.md) - Foundation implementation guide
  - [Evolution Strategy](docs/roadmap/OS_EVOLUTION_STRATEGY.md) - 36-week development roadmap
- **Boot**: Bootloader and startup documentation
  - [Multiboot Fix](docs/boot/MULTIBOOT_FIX.md) - GRUB multiboot header technical details
- **Virtualization**: Running OpenOS in VMs
  - [VirtualBox Quickstart](docs/virtualization/VIRTUALBOX_QUICKSTART.md) - Quick start guide
  - [VirtualBox Troubleshooting](docs/virtualization/VIRTUALBOX_TROUBLESHOOTING.md) - Troubleshooting guide
- **Refactoring**: Code reorganization history
  - [Refactoring Guide](docs/refactoring/REFACTORING.md) - Migration information
- **Learning**: Educational materials and progress tracking
  - [Learning Index](docs/learning/LEARNING_INDEX.md) - Current focus and completed topics
  - [Development Journal](docs/journal/DEVELOPMENT_LOG.md) - Development progress log
  - [Chapters](docs/chapters/) - Structured learning by topic

##  Project Stats

- **Lines of Code**: ~2,400 (kernel + documentation)
- **Source Files**: 33 (organized in 7 directories)
- **Exception Handlers**: 32 (all x86 exceptions)
- **Memory Management**: PMM + VMM (fully implemented)
- **Drivers**: Console (VGA), Keyboard (PS/2), Timer (PIT)
- **Supported Platforms**: QEMU, VirtualBox, Bochs
- **Documentation**: 20,000+ lines (including architecture docs)

## License
MIT License — free to use, modify, and contribute.
