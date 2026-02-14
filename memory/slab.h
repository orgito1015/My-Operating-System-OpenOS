#ifndef OPENOS_MEMORY_SLAB_H
#define OPENOS_MEMORY_SLAB_H

#include <stddef.h>

typedef struct slab {
    void *free_list;
    size_t obj_size;
} slab_t;

slab_t* slab_create(size_t obj_size);
void* slab_alloc(slab_t *slab);
void slab_free(slab_t *slab, void *ptr);

#endif
