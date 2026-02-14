# OpenOS Root Makefile
# Builds the kernel in Kernel2.0 directory

.PHONY: all clean run iso run-vbox help

all:
	@echo "Building OpenOS kernel..."
	$(MAKE) -C Kernel2.0

clean:
	@echo "Cleaning build artifacts..."
	$(MAKE) -C Kernel2.0 clean
	@rm -f openos.iso
	@rm -rf iso

run: all
	@echo "Running OpenOS in QEMU..."
	@chmod +x tools/run-qemu.sh
	@./tools/run-qemu.sh

iso: all
	@echo "Creating bootable ISO image..."
	@chmod +x tools/create-iso.sh
	@./tools/create-iso.sh

run-iso: iso
	@echo "Running OpenOS ISO in QEMU..."
	@if command -v qemu-system-i386 &> /dev/null; then \
		qemu-system-i386 -cdrom openos.iso; \
	else \
		echo "Error: qemu-system-i386 not found"; \
		exit 1; \
	fi

run-vbox: iso
	@echo "Running OpenOS in VirtualBox..."
	@chmod +x tools/run-virtualbox.sh
	@./tools/run-virtualbox.sh

help:
	@echo "OpenOS Build System"
	@echo "-------------------"
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the kernel (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Build and run in QEMU (via bootable ISO)"
	@echo "  iso      - Create bootable ISO image with GRUB"
	@echo "  run-iso  - Build ISO and run in QEMU"
	@echo "  run-vbox - Build ISO and run in VirtualBox"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build the kernel"
	@echo "  make clean        # Clean build files"
	@echo "  make run          # Build and run in QEMU"
	@echo "  make iso          # Create bootable ISO"
	@echo "  make run-vbox     # Build and run in VirtualBox"
	@echo ""
	@echo "For more information, see README.md"