# OpenOS

OpenOS is an educational, open-source operating system built from scratch for the x86 architecture.

The goal is to build a small, understandable OS **from zero**, inspired by hobby OS projects like MyraOS, xv6, and OSDev examples — but implemented with **our own code**, fully documented, and open for community contribution.

## 🎯 Mission
To create a collaborative OS development environment where students, beginners, and low-level enthusiasts can learn:

- How CPUs boot an OS  
- What a kernel actually does  
- How memory, interrupts, and drivers work  
- How processes and syscalls operate  
- How filesystems and user programs work  

All with clean, simple, modern C + Assembly code.

## 🚀 Features (planned)
- Multiboot-compatible 32-bit kernel  
- VGA text output  
- Interrupt handling (IDT, ISRs, IRQs)  
- Physical & virtual memory management (paging)  
- Kernel heap allocator  
- Basic drivers (keyboard, PIT, screen)  
- Process abstraction + simple scheduler  
- Syscalls + userland programs  
- (Later) Toy filesystem + maybe a GUI  

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
OpenOS - Educational Kernel Prototype
-------------------------------------
Running in 32-bit protected mode.
Initializing interrupts...
Keyboard initialized. Type something!

OpenOS> _
```

Type on your keyboard and press Enter to interact with the shell!

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

**Troubleshooting:** If you encounter any issues with VirtualBox, see the [VirtualBox Troubleshooting Guide](docs/VIRTUALBOX_TROUBLESHOOTING.md).

#### Option 3: ISO in QEMU

To test the ISO image in QEMU:

```bash
make run-iso
```

### Cleaning

To remove build artifacts:

```bash
make clean
```

## 🤝 Contributing

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

## 📚 Documentation

Additional documentation can be found in the `/docs` directory:

- `VIRTUALBOX_QUICKSTART.md` - Quick start guide for VirtualBox
- `VIRTUALBOX_TROUBLESHOOTING.md` - VirtualBox setup and troubleshooting
- `architecture.md` - System architecture overview
- `roadmap.md` - Future development plans
- `Kernel2.0/README.md` - Detailed kernel documentation

## 📄 License
MIT License — free to use, modify, and contribute.
