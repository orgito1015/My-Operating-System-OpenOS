/*
 * OpenOS - Kernel Heap Allocator
 *
 * Bump-pointer allocator implementing the kmalloc/kfree interface
 * declared in heap.h.  A static 64 KiB arena is used when heap_init()
 * has not been called; once heap_init() is called the supplied region
 * is used instead.
 *
 * Limitations:
 * - kfree is a no-op (bump allocator does not reclaim memory)
 * - Total allocation is bounded by the arena size
 */

#include "heap.h"
#include <stdint.h>

/* Default static heap arena (used before heap_init() is called) */
static uint8_t default_heap[65536];

/* Current heap state */
static uint8_t *heap_mem   = default_heap;
static size_t   heap_total = sizeof(default_heap);
static size_t   heap_pos   = 0;

/* Initialize the kernel heap with a caller-supplied memory region */
void heap_init(void *start, size_t size) {
    heap_mem   = (uint8_t *)start;
    heap_total = size;
    heap_pos   = 0;
}

/* Allocate size bytes aligned to alignment (must be a power of two) */
void *kmalloc_aligned(size_t size, size_t alignment) {
    /* Round up heap_pos to the requested alignment */
    heap_pos = (heap_pos + alignment - 1) & ~(alignment - 1);

    if (heap_pos + size > heap_total) return NULL;

    void *ptr = &heap_mem[heap_pos];
    heap_pos += size;
    return ptr;
}

/* Allocate size bytes with 8-byte alignment */
void *kmalloc(size_t size) {
    return kmalloc_aligned(size, 8);
}

/* Free a previously allocated block (no-op for bump allocator) */
void kfree(void *ptr) {
    (void)ptr;
}
