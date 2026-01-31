TARGET = openos

# Try to use cross-compiler first, fall back to regular gcc
CC := $(shell command -v i686-elf-gcc 2> /dev/null || echo gcc)
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32 -fno-pic -fno-stack-protector -nostdlib -nostartfiles -nodefaultlibs
ASFLAGS = $(CFLAGS)
LDFLAGS = -m32 -ffreestanding -nostdlib -static -Wl,--nmagic -Wl,-z,noexecstack

all: $(TARGET).bin

boot.o: boot.S
	$(CC) $(ASFLAGS) -c $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).bin: boot.o kernel.o linker.ld
	$(CC) -T linker.ld -o $@ $(LDFLAGS) boot.o kernel.o

run: $(TARGET).bin
	qemu-system-i386 -kernel $(TARGET).bin

clean:
	rm -f *.o $(TARGET).bin
