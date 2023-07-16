#include <stdlib.h>

#include "../allocator.h"

static void *default_alloc(Allocator alloc, size_t size);
static void *default_realloc(Allocator alloc, void *ptr, size_t size);
static void default_dealloc(Allocator alloc, void *ptr);

Allocator allocator_new() {
    return (Allocator) {
        .ctx = NULL,
        .allocate = default_alloc,
        .reallocate = default_realloc,
        .deallocate = default_dealloc
    };
}

static void *default_alloc(Allocator alloc, size_t size) {
    (void) alloc;
    return malloc(size);
}

static void *default_realloc(Allocator alloc, void *ptr, size_t size) {
    (void) alloc;
    return realloc(ptr, size);
}

static void default_dealloc(Allocator alloc, void *ptr) {
    (void) alloc;
    free(ptr);
}
