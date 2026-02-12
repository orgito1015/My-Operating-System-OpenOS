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
- **qemu-system-i386** (for testing)

#### Installing Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt-get install gcc-multilib nasm make qemu-system-x86
```

**Arch Linux:**
```bash
sudo pacman -S gcc nasm make qemu-system-x86
```

**macOS (with Homebrew):**
```bash
brew install i686-elf-gcc nasm make qemu
```

### Building

From the project root directory:

```bash
make
```

This will compile the kernel and produce `Kernel2.0/openos.bin`.

### Running

To build and run the kernel in QEMU:

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

- `architecture.md` - System architecture overview
- `roadmap.md` - Future development plans
- `Kernel2.0/README.md` - Detailed kernel documentation

## 📄 License
MIT License — free to use, modify, and contribute.
