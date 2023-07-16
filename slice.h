#ifndef SLICE_H
#define SLICE_H

#include <stddef.h>

#include "iterator.h"

typedef struct {
    void *array;
    size_t size;
    size_t elem_size;
} Slice;

#define slice_new(elem_type, arr, n_elems)                 \
    (Slice) {                                              \
        .array = arr,                                      \
        .size = n_elems,                                   \
        .elem_size = sizeof(elem_type)                     \
    }

Iterator slice_iter(Slice *slice);


#endif // SLICE_H
