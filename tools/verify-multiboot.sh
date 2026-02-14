#!/bin/bash
#
# OpenOS Multiboot Header Verification Tool
# Verifies that the kernel binary has a valid multiboot header for GRUB
#

set -e

KERNEL_BIN="${1:-Kernel2.0/openos.bin}"
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=========================================="
echo "OpenOS Multiboot Header Verification"
echo -e "==========================================${NC}"
echo ""

# Check if kernel binary exists
if [ ! -f "$KERNEL_BIN" ]; then
    echo -e "${RED}✗ Error: Kernel binary not found at $KERNEL_BIN${NC}"
    echo "  Please build the kernel first with: make"
    exit 1
fi

echo -e "${BLUE}Checking kernel binary: $KERNEL_BIN${NC}"
echo ""

# 1. Check with grub-file
echo -e "${YELLOW}[1/5] GRUB Multiboot Detection${NC}"
if command -v grub-file &> /dev/null; then
    if grub-file --is-x86-multiboot "$KERNEL_BIN"; then
        echo -e "   ${GREEN}✓ GRUB successfully detects multiboot header${NC}"
        GRUB_CHECK="PASS"
    else
        echo -e "   ${RED}✗ GRUB cannot detect multiboot header${NC}"
        GRUB_CHECK="FAIL"
    fi
else
    echo -e "   ${YELLOW}⚠ grub-file not found (install grub-pc-bin)${NC}"
    GRUB_CHECK="SKIP"
fi
echo ""

# 2. Check ELF sections
echo -e "${YELLOW}[2/5] ELF Section Layout${NC}"
if command -v objdump &> /dev/null; then
    TEXT_INFO=$(objdump -h "$KERNEL_BIN" | grep "\.text" | head -1)
    if echo "$TEXT_INFO" | grep -q "00000080"; then
        echo -e "   ${GREEN}✓ .text section starts at correct offset (0x80)${NC}"
        SECTION_CHECK="PASS"
    else
        echo -e "   ${RED}✗ .text section not at expected offset${NC}"
        SECTION_CHECK="FAIL"
    fi
    echo "   Section info: $(echo $TEXT_INFO | awk '{print $2, $3, $4, $5}')"
else
    echo -e "   ${YELLOW}⚠ objdump not found${NC}"
    SECTION_CHECK="SKIP"
fi
echo ""

# 3. Check ELF program headers
echo -e "${YELLOW}[3/5] ELF Program Headers (LOAD segments)${NC}"
if command -v readelf &> /dev/null; then
    LOAD_INFO=$(readelf -l "$KERNEL_BIN" | grep "LOAD" | head -1)
    if echo "$LOAD_INFO" | grep -q "0x000080"; then
        echo -e "   ${GREEN}✓ LOAD segment starts at correct offset (0x80)${NC}"
        LOAD_CHECK="PASS"
    else
        echo -e "   ${RED}✗ LOAD segment not at expected offset${NC}"
        LOAD_CHECK="FAIL"
    fi
    echo "   $(echo $LOAD_INFO | awk '{print $2, $3, $4, $5, $6, $7}')"
else
    echo -e "   ${YELLOW}⚠ readelf not found${NC}"
    LOAD_CHECK="SKIP"
fi
echo ""

# 4. Check multiboot header position with Python
echo -e "${YELLOW}[4/5] Multiboot Header Content${NC}"
if command -v python3 &> /dev/null; then
    RESULT=$(python3 << PYEOF
import struct
import sys

try:
    with open('$KERNEL_BIN', 'rb') as f:
        # Search for multiboot magic in first 8KB
        data = f.read(8192)
        magic = 0x1BADB002
        magic_bytes = struct.pack('<I', magic)
        
        offset = data.find(magic_bytes)
        if offset != -1:
            f.seek(offset)
            magic, flags, checksum = struct.unpack('<III', f.read(12))
            
            print(f"   Offset: 0x{offset:X} ({offset} bytes)")
            
            magic_ok = magic == 0x1BADB002
            flags_ok = flags == 0x00000003
            checksum_ok = checksum == (-(magic + flags) & 0xFFFFFFFF)
            within_8kb = offset < 8192
            
            print(f"   Magic:    0x{magic:08X} {'✓' if magic_ok else '✗'}")
            print(f"   Flags:    0x{flags:08X} {'✓' if flags_ok else '✗'}")
            print(f"   Checksum: 0x{checksum:08X} {'✓' if checksum_ok else '✗'}")
            print(f"   Within 8KB: {'✓ YES' if within_8kb else '✗ NO'}")
            
            if magic_ok and flags_ok and checksum_ok and within_8kb:
                print("PASS")
            else:
                print("FAIL")
        else:
            print("   ✗ Multiboot magic not found in first 8KB!")
            print("FAIL")
except Exception as e:
    print(f"   ✗ Error: {e}")
    print("FAIL")
PYEOF
    )
    
    if echo "$RESULT" | grep -q "PASS"; then
        echo -e "${GREEN}$(echo "$RESULT" | grep -v "PASS")${NC}"
        HEADER_CHECK="PASS"
    elif echo "$RESULT" | grep -q "FAIL"; then
        echo -e "${RED}$(echo "$RESULT" | grep -v "FAIL")${NC}"
        HEADER_CHECK="FAIL"
    else
        echo "$RESULT"
        HEADER_CHECK="UNKNOWN"
    fi
else
    echo -e "   ${YELLOW}⚠ python3 not found${NC}"
    HEADER_CHECK="SKIP"
fi
echo ""

# 5. Hexdump verification
echo -e "${YELLOW}[5/5] Binary Verification (hexdump)${NC}"
if command -v hexdump &> /dev/null; then
    HEXDATA=$(hexdump -C "$KERNEL_BIN" -s 0x80 -n 12)
    echo "$HEXDATA"
    if echo "$HEXDATA" | grep -q "02 b0 ad 1b 03 00 00 00"; then
        echo -e "   ${GREEN}✓ Multiboot header bytes verified${NC}"
        HEX_CHECK="PASS"
    else
        echo -e "   ${RED}✗ Multiboot header bytes incorrect${NC}"
        HEX_CHECK="FAIL"
    fi
else
    echo -e "   ${YELLOW}⚠ hexdump not found${NC}"
    HEX_CHECK="SKIP"
fi
echo ""

# Summary
echo -e "${BLUE}=========================================="
echo "Verification Summary"
echo -e "==========================================${NC}"
echo ""

PASS_COUNT=0
FAIL_COUNT=0
SKIP_COUNT=0

for CHECK in "$GRUB_CHECK" "$SECTION_CHECK" "$LOAD_CHECK" "$HEADER_CHECK" "$HEX_CHECK"; do
    case "$CHECK" in
        PASS) ((PASS_COUNT++)) ;;
        FAIL) ((FAIL_COUNT++)) ;;
        SKIP) ((SKIP_COUNT++)) ;;
    esac
done

# Print results
print_check() {
    local name="$1"
    local status="$2"
    printf "%-18s" "$name:"
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}✓ PASS${NC}"
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}✗ FAIL${NC}"
    else
        echo -e "${YELLOW}⚠ SKIP${NC}"
    fi
}

print_check "GRUB Detection" "$GRUB_CHECK"
print_check "Section Layout" "$SECTION_CHECK"
print_check "LOAD Segments" "$LOAD_CHECK"
print_check "Header Content" "$HEADER_CHECK"
print_check "Binary Verify" "$HEX_CHECK"
echo ""
echo "Results: $PASS_COUNT passed, $FAIL_COUNT failed, $SKIP_COUNT skipped"
echo ""

if [ $FAIL_COUNT -eq 0 ]; then
    echo -e "${GREEN}✓ Kernel is ready to boot with GRUB!${NC}"
    echo ""
    echo "Next steps:"
    echo "  1. Create ISO: make iso"
    echo "  2. Test boot:  qemu-system-i386 -cdrom openos.iso"
    echo ""
    echo "For technical details, see: docs/MULTIBOOT_FIX.md"
    exit 0
else
    echo -e "${RED}✗ Multiboot header verification failed!${NC}"
    echo ""
    echo "Troubleshooting:"
    echo "  1. Rebuild kernel: make clean && make"
    echo "  2. Check linker script: Kernel2.0/linker.ld"
    echo "  3. Verify boot.S has multiboot header"
    echo "  4. See: docs/MULTIBOOT_FIX.md"
    exit 1
fi
