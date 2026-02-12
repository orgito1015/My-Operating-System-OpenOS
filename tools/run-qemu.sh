#!/bin/bash
#
# OpenOS QEMU Launcher Script
# Runs the OpenOS kernel in QEMU emulator
#

# Color output for better visibility
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Path to kernel binary
KERNEL_BIN="Kernel2.0/openos.bin"

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

# Display kernel info
echo -e "${GREEN}Starting OpenOS in QEMU...${NC}"
echo "Kernel: $KERNEL_BIN"
echo "Press Ctrl+Alt+G to release mouse/keyboard from QEMU"
echo "Press Ctrl+C in terminal to quit"
echo ""

# Launch QEMU with the kernel
# -kernel: Direct kernel boot (Multiboot)
# -serial stdio: Redirect serial output to terminal (for debugging)
# -display gtk: Use GTK display (better than SDL on some systems)
qemu-system-i386 -kernel "$KERNEL_BIN"

echo -e "${YELLOW}QEMU exited${NC}"