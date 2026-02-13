#!/bin/bash
#
# OpenOS ISO Image Creator
# Creates a bootable ISO image with GRUB bootloader for VirtualBox/physical hardware
#

set -e  # Exit on error

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Paths
KERNEL_BIN="Kernel2.0/openos.bin"
ISO_DIR="iso"
ISO_BOOT_DIR="${ISO_DIR}/boot"
ISO_GRUB_DIR="${ISO_DIR}/boot/grub"
ISO_OUTPUT="openos.iso"

echo -e "${GREEN}Creating bootable ISO image for OpenOS...${NC}"

# Check if kernel binary exists
if [ ! -f "$KERNEL_BIN" ]; then
    echo -e "${RED}Error: Kernel binary not found at $KERNEL_BIN${NC}"
    echo "Please run 'make' first to build the kernel."
    exit 1
fi

# Check for required tools
if ! command -v grub-mkrescue &> /dev/null; then
    echo -e "${RED}Error: grub-mkrescue not found${NC}"
    echo "Please install GRUB tools:"
    echo "  Ubuntu/Debian: sudo apt-get install grub-pc-bin xorriso"
    echo "  Arch Linux:    sudo pacman -S grub xorriso"
    echo "  macOS:         brew install grub xorriso"
    exit 1
fi

if ! command -v xorriso &> /dev/null; then
    echo -e "${RED}Error: xorriso not found${NC}"
    echo "Please install xorriso:"
    echo "  Ubuntu/Debian: sudo apt-get install xorriso"
    echo "  Arch Linux:    sudo pacman -S xorriso"
    echo "  macOS:         brew install xorriso"
    exit 1
fi

# Clean up any existing ISO directory
echo "Cleaning up old ISO directory..."
rm -rf "$ISO_DIR"

# Create ISO directory structure
echo "Creating ISO directory structure..."
mkdir -p "$ISO_BOOT_DIR"
mkdir -p "$ISO_GRUB_DIR"

# Copy kernel binary
echo "Copying kernel binary..."
cp "$KERNEL_BIN" "$ISO_BOOT_DIR/"

# Copy GRUB configuration
echo "Copying GRUB configuration..."
if [ -f "grub.cfg" ]; then
    cp "grub.cfg" "$ISO_GRUB_DIR/"
else
    echo -e "${YELLOW}Warning: grub.cfg not found, creating default configuration${NC}"
    cat > "$ISO_GRUB_DIR/grub.cfg" << EOF
set timeout=3
set default=0

menuentry "OpenOS" {
    multiboot /boot/openos.bin
    boot
}
EOF
fi

# Create ISO image with GRUB
echo "Creating ISO image with grub-mkrescue..."
grub-mkrescue -o "$ISO_OUTPUT" "$ISO_DIR" 2>&1 | tee /tmp/grub-mkrescue.log | grep -v "warning: Attempting to install GRUB to a" || true
if [ -f /tmp/grub-mkrescue.log ] && grep -qi "error" /tmp/grub-mkrescue.log; then
    echo -e "${RED}Errors detected during ISO creation:${NC}"
    grep -i "error" /tmp/grub-mkrescue.log
    rm -f /tmp/grub-mkrescue.log
    exit 1
fi
rm -f /tmp/grub-mkrescue.log

# Check if ISO was created successfully
if [ -f "$ISO_OUTPUT" ]; then
    ISO_SIZE=$(du -h "$ISO_OUTPUT" | cut -f1)
    echo -e "${GREEN}Success! ISO image created: $ISO_OUTPUT ($ISO_SIZE)${NC}"
    echo ""
    echo "You can now:"
    echo "  - Test in QEMU:      qemu-system-i386 -cdrom $ISO_OUTPUT"
    echo "  - Run in VirtualBox: Create a VM and attach $ISO_OUTPUT as CD-ROM"
    echo "  - Burn to USB/CD:    Use dd or a tool like Rufus/Etcher"
else
    echo -e "${RED}Error: Failed to create ISO image${NC}"
    exit 1
fi

# Clean up temporary directory
echo "Cleaning up temporary files..."
rm -rf "$ISO_DIR"

echo -e "${GREEN}Done!${NC}"
