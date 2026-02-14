# OpenOS VirtualBox Troubleshooting Guide

This guide helps you resolve common issues when running OpenOS in VirtualBox.

## Table of Contents
1. [ISO Creation Issues](#iso-creation-issues)
2. [VirtualBox Boot Issues](#virtualbox-boot-issues)
3. [Display Issues](#display-issues)
4. [Keyboard Issues](#keyboard-issues)
5. [General Tips](#general-tips)

## ISO Creation Issues

### Error: `grub-mkrescue not found`
**Solution:** Install GRUB tools:
```bash
# Ubuntu/Debian
sudo apt-get install grub-pc-bin

# Arch Linux
sudo pacman -S grub

# macOS
brew install grub
```

### Error: `xorriso not found`
**Solution:** Install xorriso:
```bash
# Ubuntu/Debian
sudo apt-get install xorriso

# Arch Linux
sudo pacman -S xorriso

# macOS
brew install xorriso
```

### Error: `mformat invocation failed`
**Solution:** Install mtools:
```bash
# Ubuntu/Debian
sudo apt-get install mtools

# Arch Linux
sudo pacman -S mtools

# macOS
brew install mtools
```

### Error: `Kernel binary not found`
**Solution:** Build the kernel first:
```bash
make clean
make
make iso
```

## VirtualBox Boot Issues

### VM doesn't boot / stuck at black screen
**Possible causes and solutions:**

1. **ISO not attached properly:**
   - Open VM settings
   - Go to Storage
   - Ensure `openos.iso` is attached to the IDE controller
   - Ensure CD-ROM is set as first boot device in System → Boot Order

2. **Virtualization features not enabled:**
   - Open VM settings
   - Go to System → Acceleration
   - Enable VT-x/AMD-V
   - Enable Nested Paging (if available)

3. **Incorrect VM type:**
   - VM Type should be "Other"
   - Version should be "Other/Unknown" or "Other/Unknown (32-bit)"

### Error: "FATAL: No bootable medium found!"
**Solution:**
1. Ensure ISO is attached to IDE controller (not SATA)
2. Ensure CD-ROM is set as first boot device
3. Recreate the ISO: `make clean && make iso`
4. Re-attach the ISO to the VM

### GRUB boots but kernel doesn't start
**Solution:**
1. Check that the kernel was built correctly: `ls -lh Kernel2.0/openos.bin`
2. Rebuild everything: `make clean && make && make iso`
3. Check GRUB configuration in `grub.cfg`:
   ```
   menuentry "OpenOS" {
       multiboot /boot/openos.bin
       boot
   }
   ```

## Display Issues

### Screen is too small / text is hard to read
**Solution:**
1. VirtualBox uses 80x25 VGA text mode by default
2. You can adjust the VirtualBox window size
3. Enable scaled mode: View → Scaled Mode (Host+C)

### Nothing appears on screen
**Solution:**
1. Ensure VM has allocated at least 16 MB of video memory:
   - Settings → Display → Video Memory: 16 MB or more
2. Try different graphics controllers:
   - Settings → Display → Graphics Controller: Try VMSVGA or VBoxVGA

### Cursor is invisible
**Note:** This is expected behavior. The OS uses VGA text mode without a visible cursor currently.

## Keyboard Issues

### Keyboard not responding
**Solutions:**
1. Click inside the VirtualBox window to capture input
2. Check that keyboard driver initialized (you should see "Keyboard initialized" message)
3. Try pressing Ctrl+Alt+G to release and re-capture the keyboard
4. Restart the VM

### Wrong keyboard layout
**Note:** OpenOS currently uses US keyboard layout only. Other layouts are not yet supported.

### Special keys don't work (F1-F12, etc.)
**Note:** Only basic alphanumeric keys and Enter are currently supported. Function keys and other special keys will be added in future versions.

## General Tips

### Testing Before VirtualBox
Before testing in VirtualBox, verify the ISO works in QEMU:
```bash
make run-iso
```
This is faster and easier for debugging.

### Checking ISO integrity
Verify the ISO was created correctly:
```bash
file openos.iso
# Should output: ISO 9660 CD-ROM filesystem data (DOS/MBR boot sector)
```

### VM Performance
For best performance:
- Allocate at least 512 MB RAM (this is the recommended minimum)
- For smoother operation, consider 1024 MB RAM
- Enable VT-x/AMD-V hardware virtualization
- Enable PAE/NX
- Disable unnecessary features (USB, audio, network) in VM settings

### Logs and Debugging
1. Check VirtualBox logs:
   - VM → Machine → Show Log
   
2. Enable serial port output:
   - Settings → Serial Ports → Enable Port 1
   - Port Mode: TCP
   - Connect to serial output for kernel debugging

### Clean Build
If things are not working, try a clean rebuild:
```bash
make clean
make
make iso
```

## Still Having Issues?

If you're still experiencing problems:

1. **Verify your environment:**
   ```bash
   # Check installed tools
   which grub-mkrescue xorriso mtools gcc
   
   # Check GCC version
   gcc --version
   
   # Check that 32-bit compilation works
   gcc -m32 -v
   ```

2. **Try the direct kernel boot method:**
   ```bash
   make run
   ```
   If this works but VirtualBox doesn't, the issue is with ISO creation or VirtualBox configuration.

3. **Check the build output:**
   Look for warnings or errors during compilation that might indicate problems.

4. **File an issue:**
   If none of these solutions work, please file an issue on GitHub with:
   - Your OS and version
   - VirtualBox version
   - Output of `make clean && make && make iso`
   - Screenshots of any error messages
   - Contents of VirtualBox log file

## Quick Reference Commands

```bash
# Build everything from scratch
make clean && make && make iso

# Test in QEMU (fast)
make run-iso

# Run in VirtualBox (automatic)
make run-vbox

# Manually check ISO
file openos.iso
ls -lh openos.iso

# Manual VirtualBox VM creation
VBoxManage createvm --name "OpenOS" --ostype "Other" --register
VBoxManage modifyvm "OpenOS" --memory 512 --vram 16
VBoxManage storagectl "OpenOS" --name "IDE" --add ide
VBoxManage storageattach "OpenOS" --storagectl "IDE" --port 0 --device 0 --type dvddrive --medium openos.iso
VBoxManage startvm "OpenOS"
```

## Known Limitations

Current version limitations:
- Text mode only (no graphics)
- Basic keyboard support (no function keys, no international layouts)
- No file system
- No networking
- No storage devices

These are planned features for future releases!
