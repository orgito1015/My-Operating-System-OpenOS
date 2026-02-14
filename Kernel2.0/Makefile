# OpenOS Kernel Makefile
# Builds the kernel binary from C and assembly sources

# Target binary name
TARGET = openos

# Compiler selection: prefer cross-compiler, fallback to native gcc
CC := $(shell command -v i686-elf-gcc 2> /dev/null || echo gcc)

# Compiler flags for freestanding kernel development
CFLAGS = -std=gnu99          # Use GNU C99 standard
CFLAGS += -ffreestanding     # Kernel environment (no hosted libs)
CFLAGS += -O2                # Optimization level 2
CFLAGS += -Wall -Wextra      # Enable all warnings
CFLAGS += -m32               # 32-bit x86 target
CFLAGS += -fno-pic           # No position-independent code
CFLAGS += -fno-stack-protector  # No stack canaries (not available in kernel)
CFLAGS += -nostdlib          # Don't link standard library
CFLAGS += -nostartfiles      # Don't use standard startup files
CFLAGS += -nodefaultlibs     # Don't use default libraries

# Assembly flags (same as C flags for consistency)
ASFLAGS = $(CFLAGS)

# Linker flags
LDFLAGS = -ffreestanding     # Freestanding environment
LDFLAGS += -nostdlib         # No standard library
LDFLAGS += -static           # Static linking only
LDFLAGS += -Wl,--nmagic      # Don't align data segments to page boundaries
LDFLAGS += -Wl,-z,noexecstack  # Mark stack as non-executable

# Object files to link
OBJS = boot.o kernel.o idt.o pic.o isr.o keyboard.o vmm.o exceptions_asm.o exceptions.o pmm.o timer.o

# Default target: build the kernel
all: $(TARGET).bin

# Build boot loader from assembly
boot.o: boot.S
	$(CC) $(ASFLAGS) -c $< -o $@

# Build main kernel
kernel.o: kernel.c idt.h pic.h isr.h keyboard.h exceptions.h timer.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build interrupt descriptor table
idt.o: idt.c idt.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build PIC driver
pic.o: pic.c pic.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build interrupt service routines
isr.o: isr.S
	$(CC) $(ASFLAGS) -c $< -o $@

# Build keyboard driver
keyboard.o: keyboard.c keyboard.h pic.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build virtual memory manager
vmm.o: vmm.c vmm.h pmm.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build exception handler assembly stubs
exceptions_asm.o: exceptions.S
	$(CC) $(ASFLAGS) -c $< -o $@

# Build exception handlers
exceptions.o: exceptions.c exceptions.h idt.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build physical memory manager
pmm.o: pmm.c pmm.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build timer driver
timer.o: timer.c timer.h pic.h
	$(CC) $(CFLAGS) -c $< -o $@

# Link all objects into final kernel binary
$(TARGET).bin: $(OBJS) linker.ld
	$(CC) -T linker.ld -o $@ -m32 $(LDFLAGS) $(OBJS)

# Run kernel in QEMU
# Note: Direct kernel boot may not work with QEMU 7.0+ due to PVH ELF Note requirement
# For best compatibility, use 'make run' from the root directory which boots via ISO
run: $(TARGET).bin
	@echo "Note: If you get 'Error loading uncompressed kernel', use 'make run' from root directory"
	qemu-system-i386 -kernel $(TARGET).bin

# Clean build artifacts
clean:
	rm -f *.o $(TARGET).bin

# Show help
help:
	@echo "OpenOS Kernel Build System"
	@echo "=========================="
	@echo "Targets:"
	@echo "  all     - Build the kernel (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  run     - Build and run in QEMU"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Compiler: $(CC)"

.PHONY: all clean run help
