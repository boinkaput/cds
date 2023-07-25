/**
 * @file slice.h
 * @brief Definition and functions for working with slices,
 * a view over an array.
 */

#ifndef SLICE_H
#define SLICE_H

#include <stddef.h>

/**
 * @struct Slice
 * @brief Represents a slice, a view over an array.
 */
typedef struct {
    /** Pointer to the array represented by the slice */
    void *array;

    /** Number of elements in the slice */
    size_t size;

    /** Size of each element in the slice */
    size_t elem_size;
} Slice;

/**
 * @brief Creates a new Slice object representing a view over an array.
 * @param elem_type The type of elements in the array.
 * @param arr Pointer to the array to be represented by the Slice.
 * @param n_elems The number of elements in the array.
 * @return The created Slice object.
 */
#define slice_new(elem_type, arr, n_elems)                                     \
    (Slice) {                                                                  \
        .array = arr,                                                          \
        .size = n_elems,                                                       \
        .elem_size = sizeof(elem_type)                                         \
    }

/**
 * @brief Casts and returns the internal array of a Slice to a type.
 * @param slice The Slice to cast.
 * @param elem_type The type of the array elements to cast to.
 * @return A pointer to the casted array.
 * @remark The macro performs an assert check to ensure that the
 * sizeof(elem_type) == slice.elem_size.s.
 */
#define slice_cast(slice, elem_type) ({                                        \
    ASSERT(sizeof(elem_type) == slice.elem_size,                               \
           "sizeof(elem_type) (is %zu) does not match"                         \
           " slice.elem_size (is %zu)",                                        \
           sizeof(elem_type), slice.elem_size);                                \
    ((elem_type *) slice.array);                                               \
})


#endif // SLICE_H
