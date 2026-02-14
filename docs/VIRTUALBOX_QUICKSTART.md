# OpenOS VirtualBox Quick Start Guide

This guide will help you quickly get OpenOS running in VirtualBox.

## Prerequisites

Make sure you have the following installed:

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y gcc-multilib nasm make grub-pc-bin xorriso mtools virtualbox
```

### Arch Linux
```bash
sudo pacman -Sy gcc nasm make grub xorriso mtools virtualbox
```

### macOS
```bash
brew install i686-elf-gcc nasm make grub xorriso mtools
brew install --cask virtualbox
```

## Quick Start (Automated)

The easiest way to run OpenOS in VirtualBox is using the automated script:

```bash
# Clone the repository (if you haven't already)
git clone https://github.com/orgito1015/My-Operating-System-OpenOS.git
cd My-Operating-System-OpenOS

# Build and run in VirtualBox (one command!)
make run-vbox
```

This will:
1. ✓ Build the kernel
2. ✓ Create a bootable ISO image
3. ✓ Create and configure a VirtualBox VM
4. ✓ Start the VM automatically

The VirtualBox window will open, and you'll see OpenOS boot!

## Manual Setup

If you prefer to set up VirtualBox manually:

### Step 1: Create the ISO

```bash
make iso
```

This creates `openos.iso` in the root directory.

### Step 2: Create a VirtualBox VM

1. Open VirtualBox
2. Click **New** to create a new VM
3. Configure:
   - **Name:** OpenOS
   - **Type:** Other
   - **Version:** Other/Unknown (32-bit)
   - **Memory:** 512 MB (or more)
   - **Hard disk:** Do not add a virtual hard disk

### Step 3: Configure the VM

1. Select the VM and click **Settings**
2. Go to **System**:
   - **Boot Order:** Check "Optical" first, uncheck "Hard Disk"
   - **Processor:** 1 CPU
   - **Enable PAE/NX:** Checked
3. Go to **Display**:
   - **Video Memory:** 16 MB or more
4. Go to **Storage**:
   - Click the empty optical drive
   - Click the disk icon on the right
   - Choose "Choose a disk file..."
   - Select `openos.iso`
5. Click **OK**

### Step 4: Start the VM

1. Select your VM
2. Click **Start**
3. Watch OpenOS boot!

## What You Should See

When OpenOS boots, you'll see:

```
GNU GRUB version 2.xx
...
[GRUB menu with "OpenOS" option]
...
OpenOS - Educational Kernel Prototype
-------------------------------------
Running in 32-bit protected mode.
Initializing interrupts...
Keyboard initialized. Type something!

OpenOS> _
```

You can type and press Enter to interact with the shell!

## Testing Before VirtualBox

Before testing in VirtualBox, you can quickly test the ISO in QEMU:

```bash
make run-iso
```

This is faster and helpful for debugging if something goes wrong.

## Common Issues

### "VBoxManage not found"
VirtualBox is not installed. Install it using the commands in Prerequisites.

### "ISO file not found"
Run `make iso` first to create the bootable ISO.

### VM won't boot
1. Make sure the ISO is attached to the IDE controller (not SATA)
2. Ensure CD-ROM is first in boot order
3. Try rebuilding: `make clean && make iso`

### Keyboard not working
1. Click inside the VirtualBox window to capture input
2. Press Ctrl+Alt+G to release/recapture if needed

For more detailed troubleshooting, see [VIRTUALBOX_TROUBLESHOOTING.md](VIRTUALBOX_TROUBLESHOOTING.md).

## Available Make Targets

```bash
make              # Build the kernel
make clean        # Remove build artifacts
make run          # Run in QEMU (direct kernel boot)
make iso          # Create bootable ISO image
make run-iso      # Test ISO in QEMU
make run-vbox     # Build and run in VirtualBox (automated)
make help         # Show all available targets
```

## Next Steps

Now that you have OpenOS running:

1. **Experiment:** Try typing commands in the shell
2. **Learn:** Check out the documentation in `/docs`
3. **Contribute:** Read [CONTRIBUTING.md](../CONTRIBUTING.md) to learn how to contribute
4. **Explore:** Look at the kernel source code in `Kernel2.0/`

## Need Help?

- 📖 [VirtualBox Troubleshooting Guide](VIRTUALBOX_TROUBLESHOOTING.md)
- 📖 [Main README](../README.md)
- 🐛 [File an Issue](https://github.com/orgito1015/My-Operating-System-OpenOS/issues)

Enjoy exploring OpenOS! 🎉
