/*
 * OpenOS - Kernel Heap Allocator
 * 
 * Provides dynamic memory allocation for the kernel (kmalloc/kfree).
 * This is a placeholder for future implementation.
 */

#ifndef OPENOS_MEMORY_HEAP_H
#define OPENOS_MEMORY_HEAP_H

#include <stddef.h>

/* Initialize the kernel heap */
void heap_init(void *start, size_t size);

/* Allocate memory from kernel heap */
void *kmalloc(size_t size);

/* Allocate aligned memory from kernel heap */
void *kmalloc_aligned(size_t size, size_t alignment);

/* Free memory allocated from kernel heap */
void kfree(void *ptr);

#endif /* OPENOS_MEMORY_HEAP_H */
