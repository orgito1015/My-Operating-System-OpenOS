#!/bin/bash
# Run OpenOS kernel in QEMU

# Check if the kernel binary exists
if [ ! -f "Kernel2.0/openos.bin" ]; then
    echo "Error: Kernel binary not found. Please run 'make' in Kernel2.0/ first."
    exit 1
fi

# Launch QEMU with the kernel
echo "Starting OpenOS in QEMU..."
qemu-system-i386 -kernel Kernel2.0/openos.bin