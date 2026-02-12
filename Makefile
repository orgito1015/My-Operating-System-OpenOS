# OpenOS Root Makefile
# Builds the kernel in Kernel2.0 directory

.PHONY: all clean run help

all:
	@echo "Building OpenOS kernel..."
	$(MAKE) -C Kernel2.0

clean:
	@echo "Cleaning build artifacts..."
	$(MAKE) -C Kernel2.0 clean

run: all
	@echo "Running OpenOS in QEMU..."
	@chmod +x tools/run-qemu.sh
	@./tools/run-qemu.sh

help:
	@echo "OpenOS Build System"
	@echo "-------------------"
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all    - Build the kernel (default)"
	@echo "  clean  - Remove build artifacts"
	@echo "  run    - Build and run in QEMU"
	@echo "  help   - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make         # Build the kernel"
	@echo "  make clean   # Clean build files"
	@echo "  make run     # Build and run in QEMU"
	@echo ""
	@echo "For more information, see README.md"