/*
 * OpenOS - Kernel Heap Allocator
 *
 * A first-fit free-list allocator with block splitting and coalescing.
 * Replaces the previous bump allocator: kfree() now genuinely reclaims
 * memory, and freed adjacent blocks are merged to limit fragmentation.
 *
 * Layout: the arena is a contiguous run of blocks. Every block carries
 * a header (size + free flag + neighbour links). kmalloc returns the
 * payload immediately after a header; kfree recovers the header by
 * subtracting the header size from the user pointer.
 *
 * Before heap_init() is called a static 64 KiB arena is used so the
 * allocator is usable very early in boot; heap_init() then re-points it
 * at a larger PMM-backed region.
 */

#include "heap.h"
#include <stdint.h>
#include <stddef.h>

/* Block header. Kept 8-byte sized so payloads stay 8-byte aligned. */
typedef struct block_header {
    size_t               size;   /* payload size in bytes              */
    uint32_t             free;   /* 1 = free, 0 = in use               */
    struct block_header *next;   /* next block in address order        */
    struct block_header *prev;   /* previous block in address order    */
} block_header_t;

#define HEADER_SIZE   (sizeof(block_header_t))
#define MIN_PAYLOAD   16
#define HEAP_ALIGN(n, a) (((n) + (a) - 1) & ~((a) - 1))

/* Default static arena used before heap_init() supplies a real region. */
static uint8_t default_heap[65536];

static uint8_t        *heap_base = default_heap;
static size_t          heap_size = sizeof(default_heap);
static block_header_t *heap_head = NULL;

/* Build a single free block spanning the whole arena. */
static void heap_build_freelist(void) {
    heap_head = (block_header_t *)heap_base;
    heap_head->size = heap_size - HEADER_SIZE;
    heap_head->free = 1;
    heap_head->next = NULL;
    heap_head->prev = NULL;
}

/*
 * Initialize the kernel heap with a caller-supplied memory region.
 * Safe to call once after the PMM is up to grant a large arena.
 */
void heap_init(void *start, size_t size) {
    if (start == NULL || size <= HEADER_SIZE + MIN_PAYLOAD) {
        /* Fall back to the static arena on a bad request. */
        heap_base = default_heap;
        heap_size = sizeof(default_heap);
    } else {
        heap_base = (uint8_t *)start;
        heap_size = size;
    }
    heap_build_freelist();
}

/* Split block so it holds exactly want bytes, returning leftover to the list. */
static void block_split(block_header_t *blk, size_t want) {
    if (blk->size < want + HEADER_SIZE + MIN_PAYLOAD) {
        return; /* not enough slack to carve a usable remainder */
    }
    block_header_t *rest = (block_header_t *)((uint8_t *)blk + HEADER_SIZE + want);
    rest->size = blk->size - want - HEADER_SIZE;
    rest->free = 1;
    rest->prev = blk;
    rest->next = blk->next;
    if (rest->next) {
        rest->next->prev = rest;
    }
    blk->size = want;
    blk->next = rest;
}

void *kmalloc_aligned(size_t size, size_t alignment) {
    if (heap_head == NULL) {
        heap_build_freelist();
    }
    if (size == 0) {
        return NULL;
    }
    if (alignment < 8) {
        alignment = 8;
    }

    size_t want = HEAP_ALIGN(size, alignment);

    for (block_header_t *blk = heap_head; blk != NULL; blk = blk->next) {
        if (blk->free && blk->size >= want) {
            block_split(blk, want);
            blk->free = 0;
            return (void *)((uint8_t *)blk + HEADER_SIZE);
        }
    }
    return NULL; /* out of heap space */
}

void *kmalloc(size_t size) {
    return kmalloc_aligned(size, 8);
}

/* Merge a free block with a free neighbour that follows it. */
static void block_coalesce(block_header_t *blk) {
    block_header_t *nxt = blk->next;
    if (nxt && nxt->free) {
        blk->size += HEADER_SIZE + nxt->size;
        blk->next = nxt->next;
        if (nxt->next) {
            nxt->next->prev = blk;
        }
    }
}

void kfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    block_header_t *blk = (block_header_t *)((uint8_t *)ptr - HEADER_SIZE);
    blk->free = 1;

    /* Coalesce forward, then backward, to keep large runs contiguous. */
    block_coalesce(blk);
    if (blk->prev && blk->prev->free) {
        block_coalesce(blk->prev);
    }
}
