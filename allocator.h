#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef struct allocator {
    void *ctx;
    void *(*allocate)(struct allocator alloc, size_t size);
    void *(*reallocate)(struct allocator alloc, void *ptr, size_t size);
    void (*deallocate)(struct allocator alloc, void *ptr);
} Allocator;

#define allocator_allocate(alloc, size) \
    alloc.allocate(alloc, size)

#define allocator_reallocate(alloc, ptr, size) \
    alloc.reallocate(alloc, ptr, size)

#define allocator_deallocate(alloc, ptr) \
    alloc.deallocate(alloc, ptr)

Allocator allocator_default();

#endif // ALLOCATOR_H
