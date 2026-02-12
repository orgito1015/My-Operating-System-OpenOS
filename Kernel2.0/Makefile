TARGET = openos

# Try to use cross-compiler first, fall back to regular gcc
CC := $(shell command -v i686-elf-gcc 2> /dev/null || echo gcc)
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32 -fno-pic -fno-stack-protector -nostdlib -nostartfiles -nodefaultlibs
ASFLAGS = $(CFLAGS)
LDFLAGS = -ffreestanding -nostdlib -static -Wl,--nmagic -Wl,-z,noexecstack

OBJS = boot.o kernel.o idt.o pic.o isr.o keyboard.o

all: $(TARGET).bin

boot.o: boot.S
	$(CC) $(ASFLAGS) -c $< -o $@

kernel.o: kernel.c idt.h pic.h isr.h keyboard.h
	$(CC) $(CFLAGS) -c $< -o $@

idt.o: idt.c idt.h
	$(CC) $(CFLAGS) -c $< -o $@

pic.o: pic.c pic.h
	$(CC) $(CFLAGS) -c $< -o $@

isr.o: isr.S
	$(CC) $(ASFLAGS) -c $< -o $@

keyboard.o: keyboard.c keyboard.h pic.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).bin: $(OBJS) linker.ld
	$(CC) -T linker.ld -o $@ -m32 $(LDFLAGS) $(OBJS)

run: $(TARGET).bin
	qemu-system-i386 -kernel $(TARGET).bin

clean:
	rm -f *.o $(TARGET).bin
