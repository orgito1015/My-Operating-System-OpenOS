# OpenOS Kernel Makefile
# Builds the kernel binary from the modular directory structure

# Target binary name and output location
TARGET = openos
OUTPUT_DIR = Kernel2.0
OUTPUT_BIN = $(OUTPUT_DIR)/$(TARGET).bin

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
CFLAGS += -I./include        # Include common headers
CFLAGS += -I./arch/x86       # Include x86 architecture headers
CFLAGS += -I./kernel         # Include kernel headers
CFLAGS += -I./memory         # Include memory headers
CFLAGS += -I./drivers        # Include driver headers

# Assembly flags (same as C flags for consistency)
ASFLAGS = $(CFLAGS)

# Linker flags
LDFLAGS = -ffreestanding     # Freestanding environment
LDFLAGS += -nostdlib         # No standard library
LDFLAGS += -static           # Static linking only
LDFLAGS += -Wl,--nmagic      # Don't align data segments to page boundaries
LDFLAGS += -Wl,-z,noexecstack  # Mark stack as non-executable

# Source directories
ARCH_DIR = arch/x86
KERNEL_DIR = kernel
MEMORY_DIR = memory
DRIVERS_DIR = drivers

# Architecture-specific object files
ARCH_OBJS = $(ARCH_DIR)/boot.o \
            $(ARCH_DIR)/idt.o \
            $(ARCH_DIR)/isr.o \
            $(ARCH_DIR)/pic.o \
            $(ARCH_DIR)/exceptions_asm.o \
            $(ARCH_DIR)/exceptions.o

# Kernel object files
KERNEL_OBJS = $(KERNEL_DIR)/kernel.o \
              $(KERNEL_DIR)/panic.o

# Memory management object files
MEMORY_OBJS = $(MEMORY_DIR)/pmm.o \
              $(MEMORY_DIR)/vmm.o

# Driver object files
DRIVERS_OBJS = $(DRIVERS_DIR)/console.o \
               $(DRIVERS_DIR)/keyboard.o \
               $(DRIVERS_DIR)/timer.o

# All object files
OBJS = $(ARCH_OBJS) $(KERNEL_OBJS) $(MEMORY_OBJS) $(DRIVERS_OBJS)

# Default target: build the kernel
all: $(OUTPUT_BIN)

# Ensure output directory exists
$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# Architecture-specific files
$(ARCH_DIR)/boot.o: $(ARCH_DIR)/boot.S
	$(CC) $(ASFLAGS) -c $< -o $@

$(ARCH_DIR)/idt.o: $(ARCH_DIR)/idt.c $(ARCH_DIR)/idt.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ARCH_DIR)/isr.o: $(ARCH_DIR)/isr.S
	$(CC) $(ASFLAGS) -c $< -o $@

$(ARCH_DIR)/pic.o: $(ARCH_DIR)/pic.c $(ARCH_DIR)/pic.h $(ARCH_DIR)/ports.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ARCH_DIR)/exceptions_asm.o: $(ARCH_DIR)/exceptions.S
	$(CC) $(ASFLAGS) -c $< -o $@

$(ARCH_DIR)/exceptions.o: $(ARCH_DIR)/exceptions.c $(ARCH_DIR)/exceptions.h $(ARCH_DIR)/idt.h
	$(CC) $(CFLAGS) -c $< -o $@

# Kernel files
$(KERNEL_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c $(KERNEL_DIR)/kernel.h
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_DIR)/panic.o: $(KERNEL_DIR)/panic.c $(KERNEL_DIR)/panic.h
	$(CC) $(CFLAGS) -c $< -o $@

# Memory management files
$(MEMORY_DIR)/pmm.o: $(MEMORY_DIR)/pmm.c $(MEMORY_DIR)/pmm.h
	$(CC) $(CFLAGS) -c $< -o $@

$(MEMORY_DIR)/vmm.o: $(MEMORY_DIR)/vmm.c $(MEMORY_DIR)/vmm.h $(MEMORY_DIR)/pmm.h
	$(CC) $(CFLAGS) -c $< -o $@

# Driver files
$(DRIVERS_DIR)/console.o: $(DRIVERS_DIR)/console.c $(DRIVERS_DIR)/console.h
	$(CC) $(CFLAGS) -c $< -o $@

$(DRIVERS_DIR)/keyboard.o: $(DRIVERS_DIR)/keyboard.c $(DRIVERS_DIR)/keyboard.h $(ARCH_DIR)/pic.h $(ARCH_DIR)/ports.h
	$(CC) $(CFLAGS) -c $< -o $@

$(DRIVERS_DIR)/timer.o: $(DRIVERS_DIR)/timer.c $(DRIVERS_DIR)/timer.h $(ARCH_DIR)/pic.h $(ARCH_DIR)/ports.h
	$(CC) $(CFLAGS) -c $< -o $@

# Link all objects into final kernel binary
$(OUTPUT_BIN): $(OUTPUT_DIR) $(OBJS) linker.ld
	$(CC) -T linker.ld -o $@ -m32 $(LDFLAGS) $(OBJS)

# Run kernel in QEMU (uses ISO boot)
run: $(OUTPUT_BIN)
	@echo "Running OpenOS in QEMU..."
	@chmod +x tools/run-qemu.sh
	@./tools/run-qemu.sh

# Create bootable ISO
iso: $(OUTPUT_BIN)
	@echo "Creating bootable ISO image..."
	@chmod +x tools/create-iso.sh
	@./tools/create-iso.sh

# Run ISO in QEMU
run-iso: iso
	@echo "Running OpenOS ISO in QEMU..."
	@if command -v qemu-system-i386 &> /dev/null; then \
		qemu-system-i386 -cdrom openos.iso; \
	else \
		echo "Error: qemu-system-i386 not found"; \
		exit 1; \
	fi

# Run in VirtualBox
run-vbox: iso
	@echo "Running OpenOS in VirtualBox..."
	@chmod +x tools/run-virtualbox.sh
	@./tools/run-virtualbox.sh

# Clean build artifacts
clean:
	rm -f $(ARCH_DIR)/*.o $(KERNEL_DIR)/*.o $(MEMORY_DIR)/*.o $(DRIVERS_DIR)/*.o
	rm -f $(OUTPUT_BIN)
	rm -f openos.iso
	rm -rf iso

# Show help
help:
	@echo "OpenOS Kernel Build System (Modular Architecture)"
	@echo "=================================================="
	@echo "Targets:"
	@echo "  all      - Build the kernel (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Build and run in QEMU (via bootable ISO)"
	@echo "  iso      - Create bootable ISO image with GRUB"
	@echo "  run-iso  - Build ISO and run in QEMU"
	@echo "  run-vbox - Build ISO and run in VirtualBox"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Directory Structure:"
	@echo "  arch/x86/   - x86 architecture-specific code"
	@echo "  kernel/     - Core kernel code"
	@echo "  memory/     - Memory management (PMM, VMM, heap)"
	@echo "  drivers/    - Hardware drivers (console, keyboard, timer)"
	@echo "  fs/         - File systems (VFS, ramfs)"
	@echo "  process/    - Process management"
	@echo "  include/    - Common headers"
	@echo ""
	@echo "Compiler: $(CC)"

.PHONY: all clean run iso run-iso run-vbox help
