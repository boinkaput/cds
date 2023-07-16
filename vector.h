#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

#include "allocator.h"
#include "iterator.h"

#define Vec(elem_type) elem_type *

#ifndef VECTOR_ALLOCATOR
#define vec_new(elem_type)                                                     \
    internal_vec_new(sizeof(elem_type), allocator_new())

#define vec_with_capacity(elem_type, capacity)                                 \
    internal_vec_with_cap(capacity, sizeof(elem_type), allocator_new())

#define vec_from_array(elem_type, array, array_size)                           \
    internal_vec_from_arr(array, array_size, sizeof(elem_type),                \
                          allocator_new())
#else
#define vec_new(elem_type, allocator)                                          \
    internal_vec_new(sizeof(elem_type), allocator)

#define vec_with_capacity(elem_type, capacity, allocator)                      \
    internal_vec_with_cap(sizeof(elem_type), capacity, allocator)

#define vec_from_array(elem_type, capacity, allocator)                         \
    internal_vec_from_arr(sizeof(elem_type), capacity, allocator)
#endif // VECTOR_ALLOCATOR

#define vec_insert(vector, elem, index)                                        \
    do {                                                                       \
        typeof(*vector) e = elem;                                              \
        vector = internal_vec_insert(vector, &e, index);                       \
    } while(0)

#define vec_push_back(vector, elem)                                            \
    do {                                                                       \
        typeof(*vector) e = elem;                                              \
        vector = internal_vec_push_back(vector, &e);                                    \
    } while(0)

#define vec_extend(vector, array, array_size)                                  \
    do {                                                                       \
        typeof(vector) arr = array;                                            \
        vector = internal_vec_extend(vector, arr, array_size);                 \
    } while (0)


size_t vec_size(void *vector);

size_t vec_capacity(void *vector);

bool vec_is_empty(void *vector);

void vec_erase(void *vector, size_t index, void *elem);

void vec_swap_erase(void *vector, size_t index, void *elem);

void vec_pop_back(void *vector, void *elem);

void vec_clear(void *vector);

void vec_free(void *vector);

void *vec_reserve(void *vector, size_t new_capacity);

void *vec_shrink(void *vector);

size_t vec_slice(void *vector, size_t start, size_t end, void *buffer);

Iterator vec_iter(void *vector);



void *internal_vec_new(size_t elem_size, Allocator alloc);

void *internal_vec_with_cap(size_t capacity, size_t elem_size, Allocator alloc);

void *internal_vec_from_arr(void *array, size_t array_size, size_t elem_size,
                            Allocator alloc);

void *internal_vec_insert(void *vector, void *elem, size_t index);

void *internal_vec_push_back(void *vector, void *elem);

void *internal_vec_extend(void *vector, void *array, size_t array_size);


#endif // VECTOR_H
