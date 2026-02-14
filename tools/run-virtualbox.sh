#!/bin/bash
#
# OpenOS VirtualBox Launcher Script
# Creates and runs OpenOS in VirtualBox
#

set -e  # Exit on error

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
VM_NAME="OpenOS"
ISO_FILE="openos.iso"
VM_MEMORY=512      # MB
VM_VRAM=16         # MB

echo -e "${GREEN}OpenOS VirtualBox Launcher${NC}"
echo "=============================="

# Check if VirtualBox is installed
if ! command -v VBoxManage &> /dev/null; then
    echo -e "${RED}Error: VirtualBox (VBoxManage) not found${NC}"
    echo "Please install VirtualBox:"
    echo "  Ubuntu/Debian: sudo apt-get install virtualbox"
    echo "  Arch Linux:    sudo pacman -S virtualbox"
    echo "  macOS:         brew install --cask virtualbox"
    echo "  Or download from: https://www.virtualbox.org/wiki/Downloads"
    exit 1
fi

# Check if ISO exists
if [ ! -f "$ISO_FILE" ]; then
    echo -e "${RED}Error: ISO file not found: $ISO_FILE${NC}"
    echo "Please run 'make iso' first to create the bootable ISO."
    exit 1
fi

# Remove existing VM if it exists
if VBoxManage list vms | grep -q "\"$VM_NAME\""; then
    echo -e "${YELLOW}Removing existing VM: $VM_NAME${NC}"
    VBoxManage unregistervm "$VM_NAME" --delete 2>/dev/null || true
fi

# Create new VM
echo "Creating VirtualBox VM: $VM_NAME"
VBoxManage createvm --name "$VM_NAME" --ostype "Other" --register

# Configure VM
echo "Configuring VM..."
VBoxManage modifyvm "$VM_NAME" \
    --memory $VM_MEMORY \
    --vram $VM_VRAM \
    --boot1 dvd \
    --boot2 none \
    --boot3 none \
    --boot4 none \
    --acpi on \
    --ioapic on \
    --cpus 1 \
    --pae on \
    --hwvirtex on

# Create and attach storage controller
echo "Setting up storage..."
VBoxManage storagectl "$VM_NAME" --name "IDE" --add ide --controller PIIX4

# Attach ISO as CD-ROM
ISO_ABSOLUTE_PATH=$(realpath "$ISO_FILE")
VBoxManage storageattach "$VM_NAME" \
    --storagectl "IDE" \
    --port 0 \
    --device 0 \
    --type dvddrive \
    --medium "$ISO_ABSOLUTE_PATH"

# Start VM
echo -e "${GREEN}Starting VM: $VM_NAME${NC}"
echo ""
echo "VirtualBox window should open shortly..."
echo "To stop the VM, close the window or use: VBoxManage controlvm $VM_NAME poweroff"
echo ""

VBoxManage startvm "$VM_NAME"

echo -e "${GREEN}VM launched successfully!${NC}"
