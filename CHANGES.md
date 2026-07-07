# OpenOS — Changes

Summary of a test / fix / feature pass over the kernel. Everything below was
built with `make` and boot-tested in QEMU (`qemu-system-i386`) through to the
interactive `OpenOS>` shell.

## Critical bug fixed — kernel did not boot (triple fault → reboot loop)

**Symptom:** on boot the CPU took a #GP (v=0d), then a double fault (v=08),
then a triple fault, resetting in a loop. Screen stayed blank.

**Root cause:** the physical memory manager never reserved the kernel's own
page frames. The kernel loads at physical `0x100000` and its image spans
~3.95 MiB (up to ~`0x4C5BC8`, including the 1 MiB PMM bitmap in BSS). `pmm_init`
marked everything below 1 MiB used and everything from `0x100000` up as *free* —
i.e. the memory the running kernel occupies. The first `pmm_alloc_page()` in
`vmm_init()` therefore returned `0x100000` and the page directory was written
straight over live kernel code/data, corrupting it and faulting.

**Fix:**
- `linker.ld`: export `kernel_start` / `kernel_end` symbols around the image.
- `memory/pmm.c`: added `reserve_range()` and reserve both page 0 (null / IVT /
  BDA) and `[kernel_start, kernel_end)` in `pmm_init`, in both the mmap and the
  no-mmap fallback paths. Used-page count is recomputed after reservation.

After the fix the allocator hands out only free frames above the kernel, paging
structures build correctly, and the kernel reaches the shell.

## Other fixes

- `kernel/kernel.c`: boot step counter was inconsistent (`[1/11]`…`[8/11]` then
  `[9/13]`…`[13/13]`). All steps now read `/13`.
- `Makefile`: the Rust `driver_config` static library is now **optional**. It
  compiles into the image but no C code references its symbols, so the kernel
  builds and boots identically with or without it. The build now detects whether
  `cargo` and the `i686-unknown-linux-musl` target are installed and skips the
  Rust step gracefully otherwise, so `make` always produces a kernel. `make
  clean` no longer fails when `cargo` is absent.

## New feature — serial console (COM1)

New `drivers/serial.c` / `drivers/serial.h` (16550 UART, 38400 8N1, with a
loopback self-test). The console driver now mirrors all output to COM1, so the
kernel is usable headless and the existing `make debug` / `make qemu-log`
targets capture a full boot/session log:

```
qemu-system-i386 -cdrom openos.iso -serial stdio
```

## New shell commands

Filesystem creation/removal and memory reporting (the VFS already supported
create/write; it just wasn't exposed):

| Command | Description |
|---------|-------------|
| `mkdir <name>...` | Create directories |
| `touch <name>...` | Create empty files |
| `write <file> <text...>` | Write text to a file (creates it if needed) |
| `rm <name>...` | Remove files / directory entries |
| `meminfo` | Report physical (PMM) and kernel heap usage |

Supporting change: `memory/heap.c` / `memory/heap.h` gained `heap_get_stats()`
(walks the free list to report total/used/free bytes and block count) so
`meminfo` can show heap state.

## Verified session (over serial)

```
OpenOS> meminfo
Physical memory (PMM, 4 KiB pages)
  total : 32736 pages (130944 KiB)
  used  : 2265 pages (9060 KiB)
  free  : 30471 pages (121884 KiB)
Kernel heap (first-fit free list)
  total : 4194288 bytes
  used  : 0 bytes
  free  : 4194288 bytes across 1 block(s)
OpenOS> mkdir projects
OpenOS> cd projects
OpenOS> write notes.txt hello openos world
OpenOS> ls -l
-        19  notes.txt
OpenOS> cat notes.txt
hello openos world
```

## Build / run

```
make            # build kernel  -> build/openos.bin
make iso        # bootable ISO  -> openos.iso
make run-iso    # build ISO and run in QEMU
make qemu-log   # run with serial + machine log to qemu.log
```

Toolchain used: `gcc -m32` (multilib), `grub-mkrescue` + `xorriso`,
`qemu-system-i386`. The Rust component additionally needs `cargo` and
`rustup target add i686-unknown-linux-musl` (optional; skipped if absent).
