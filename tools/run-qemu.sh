#!/bin/bash
#
# OpenOS QEMU Launcher Script
# Runs the OpenOS kernel in QEMU emulator via bootable ISO
#

# Color output for better visibility
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Paths
KERNEL_BIN="Kernel2.0/openos.bin"
ISO_FILE="openos.iso"

# Check if QEMU is installed
if ! command -v qemu-system-i386 &> /dev/null; then
    echo -e "${RED}Error: qemu-system-i386 not found${NC}"
    echo "Please install QEMU:"
    echo "  Ubuntu/Debian: sudo apt-get install qemu-system-x86"
    echo "  Arch Linux:    sudo pacman -S qemu-system-x86"
    echo "  macOS:         brew install qemu"
    exit 1
fi

# Check if the kernel binary exists
if [ ! -f "$KERNEL_BIN" ]; then
    echo -e "${RED}Error: Kernel binary not found at $KERNEL_BIN${NC}"
    echo "Please run 'make' first to build the kernel."
    exit 1
fi

# Build ISO if it doesn't exist or is older than the kernel
if [ ! -f "$ISO_FILE" ] || [ "$KERNEL_BIN" -nt "$ISO_FILE" ]; then
    echo -e "${YELLOW}Building bootable ISO...${NC}"
    chmod +x tools/create-iso.sh
    if ! ./tools/create-iso.sh > /dev/null 2>&1; then
        echo -e "${RED}Error: Failed to create ISO${NC}"
        echo "Required tools: grub-mkrescue, xorriso, mtools"
        echo "Install with:"
        echo "  Ubuntu/Debian: sudo apt-get install grub-pc-bin xorriso mtools"
        echo "  Arch Linux:    sudo pacman -S grub xorriso mtools"
        exit 1
    fi
fi

# Display startup info
echo -e "${GREEN}Starting OpenOS in QEMU...${NC}"
echo "Kernel: $KERNEL_BIN"
echo "Boot method: ISO with GRUB (compatible with all QEMU versions)"
echo "Press Ctrl+Alt+G to release mouse/keyboard from QEMU"
echo "Press Ctrl+C in terminal to quit"
echo ""

# Launch QEMU with ISO
# Using ISO boot is more reliable than direct kernel boot
# and works with all QEMU versions (including 7.0+)
qemu-system-i386 -cdrom "$ISO_FILE"

echo -e "${YELLOW}QEMU exited${NC}"