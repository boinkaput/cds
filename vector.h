/**
 * @file vector.h
 * @brief Definition and functions for a dynamic array container.
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <string.h>

#include "allocator.h"
#include "iterator.h"

/**
 * @brief Macro to define a vector type with the specified element type.
 * @param elem_type The type of the elements in the vector.
 * @return The defined vector type.
 * @note If elem_type is an array then make an alias using typedef before
 * using it, like this:
 * @note ```struct Person { char name[50]; int age; };```
 * @note ```typedef struct Person People[3];```
 * @note ```Vec(People) vec = vec_new(People);```
 */
#define Vec(elem_type) elem_type *

/**
 * @brief Creates a new vector with the specified element type.
 * @param elem_type The type of the elements in the vector.
 * @param vec_args Optional args, see `VecArgs` for more info.
 * @return The created vector.
 * @note `vec_args` defaults to `(VecArgs) { .cap = 0, .alloc = allocator_new() }`
 */
#define vec_new(elem_type, ...)                                                \
    internal_vec_new(                                                          \
        sizeof(elem_type),                                                     \
        (VecArgs) { .cap = 0, .alloc = allocator_new(), __VA_ARGS__ },         \
        0                                                                      \
    );

/**
 * @brief Creates a new vector from an array.
 * @param array The array containing elements to copy.
 * @param size The number of elements in the array.
 * @param vec_args Optional args, see `VecArgs` for more info.
 * @return The created vector.
 * @note `vec_args` defaults to `(VecArgs) { .cap = 0, .alloc = allocator_new() }`
 * @note If `vec_args.cap` is specified then a check is performed to see
 * if it is greater than `size`. If this is the case then the vector's
 * capacity is set to `vec_args.cap`, else `size` is used as the vector's capacity.
 */
#define vec_from_array(array, size, ...)                                       \
    memcpy(                                                                    \
        internal_vec_new(                                                      \
            sizeof(array[0]),                                                  \
            (VecArgs) { .cap = 0, .alloc = allocator_new(), __VA_ARGS__ },     \
            size                                                               \
        ),                                                                     \
        array,                                                                 \
        sizeof(array[0]) * size                                                \
    )

/**
 * @brief Creates a slice of the vector.
 * @param vector The vector.
 * @param buffer The buffer to which the slice is copied to.
 * @param vec_slice_args Optional args, see `VecSliceArgs` for more info.
 * @return The size of the slice.
 * @note Defaults to `(VecSliceArgs) { .start = 0, .end = vec_size(vector) }`
 * @note Elements are shallow copied from the vector.
 * @note `vec_slice_args.start` is inclusive, however `.end` is exclusive.
 * This means that the elements returned range from `vec_slice_args.start`
 * to `vec_slice_args.end - 1`, size of the slice would be
 * `vec_slice_args.end - vec_slice_args.start`.
 */
#define vec_slice(vector, buffer, ...)                                         \
    internal_vec_slice(                                                        \
        vector,                                                                \
        buffer,                                                                \
        (VecSliceArgs) { .start = 0, .end = vec_size(vector), __VA_ARGS__ }    \
    )

/**
 * @brief Inserts an element into the vector at the specified index.
 * @param vector The vector.
 * @param elem The element to insert.
 * @param index The index at which to insert the element.
 * @note elem is shallow copied.
 */
#define vec_insert(vector, elem, index)                                        \
    do {                                                                       \
        typeof(*vector) e = elem;                                              \
        vector = internal_vec_insert(vector, &e, index);                       \
    } while(0)

/**
 * @brief Appends an element to the back of the vector.
 * @param vector The vector.
 * @param elem The element to append.
 * @note elem is shallow copied.
 */
#define vec_push_back(vector, elem)                                            \
    do {                                                                       \
        typeof(*vector) e = elem;                                              \
        vector = internal_vec_push_back(vector, &e);                           \
    } while(0)

/**
 * @brief Extends the vector by copying elements from an array.
 * @param vector The vector.
 * @param array The array to extend the vector from.
 * @param size The number of elements in the array.
 * @note array elements are shallow copied.
 */
#define vec_extend(vector, array, size)                                        \
    do {                                                                       \
        vector = internal_vec_extend(vector, array, size);                     \
    } while (0)

/**
 * @brief Returns the number of elements in the vector.
 * @param vector The vector.
 * @return The size of the vector.
 */
size_t vec_size(void *vector);

/**
 * @brief Returns the capacity of the vector.
 * @param vector The vector.
 * @return The capacity of the vector.
 */
size_t vec_capacity(void *vector);

/**
 * @brief Checks if the vector is empty.
 * @param vector The vector.
 * @return `true` if the vector is empty, `false` otherwise.
 */
bool vec_is_empty(void *vector);

/**
 * @brief Erases an element at the specified index in the vector.
 * @param vector The vector.
 * @param index The index of the element to erase.
 * @param elem A pointer to store the erased element.
 * @note Supply NULL for `elem` if you don't care about the erased value.
 * @note The capacity of vector remains the same (i.e. free is not called).
 */
void vec_erase(void *vector, size_t index, void *elem);

/**
 * @brief Erases by swapping the element at the specified index with the
 * last element in the vector.
 * @param vector The vector.
 * @param index The index of the element to erase.
 * @param elem A pointer to store the erased element.
 * @note Supply NULL for `elem` if you don't care about the erased value.
 * @note The capacity of vector remains the same (i.e. free is not called).
 */
void vec_swap_erase(void *vector, size_t index, void *elem);

/**
 * @brief Removes the last element from the vector.
 * @param vector The vector.
 * @param elem A pointer to store the popped element.
 * @note Supply NULL for `elem` if you don't care about the erased value.
 * @note The capacity of vector remains the same (i.e. free is not called).
 */
void vec_pop_back(void *vector, void *elem);

/**
 * @brief Clears the vector, removing all elements.
 * @param vector The vector to clear.
 * @note The capacity of vector remains the same (i.e. free is not called).
 */
void vec_clear(void *vector);

/**
 * @brief Frees the memory used by the vector.
 * @param vector The vector to free.
 * @note If the vector holds references to elements on the heap then
 * it is the user's responsibility to free those objects.
 */
void vec_free(void *vector);

/**
 * @brief Reserves capacity for the vector, ensuring it can hold at least
 * the specified number of elements.
 * @param vector The vector.
 * @param new_capacity The new capacity to reserve.
 * @return A pointer to the vector with the reserved capacity.
 */
void *vec_reserve(void *vector, size_t new_capacity);

/**
 * @brief Shrinks the capacity of the vector to match its size.
 * @param vector The vector.
 * @return A pointer to the vector with the shrunk capacity.
 */
void *vec_shrink(void *vector);

/**
 * @brief Reverses the order of elements in the vector in place.
 * @param vector The vector to reverse.
 */
void vec_reverse(void *vector);

/**
 * @brief Creates an iterator for the vector.
 * @param vector The vector.
 * @return An iterator for the vector.
 */
Iterator vec_iter(void *vector);

/*----------------------------- Argument Struct -----------------------------*/

/**
 * @brief Represents optional arguments for configuring a vector.
 * @note Examples of how to use this struct:
 * @note `Vec(int) vec = vec_new(int);`
 * @note `Vec(int) vec = vec_new(int, .cap = 10);`
 * @note `Vec(int) vec = vec_new(int, .alloc = allocator_new());`
 * @note `Vec(int) vec = vec_new(int, .cap = 10, .alloc = allocator_new());`
 */
typedef struct {
    /** The capacity of the vector */
    size_t cap;

    /** The allocator for memory allocation */
    Allocator alloc;
} VecArgs;

/**
 * @brief Represents optional arguments for creating a vector slice.
 * @note Examples of how to use this struct:
 * @note `vec_slice(vec, buffer);`
 * @note `vec_slice(vec, buffer, .start = 5);`
 * @note `vec_slice(vec, buffer, .end = 8);`
 * @note `vec_slice(vec, buffer, .start = 5, .end = 8);`
 */
typedef struct {
    /** The start index of the slice */
    size_t start;

    /** The end index of the slice */
    size_t end;
} VecSliceArgs;

/*------------------------ Internal Helper Functions ------------------------*/

/**
 * @brief Internal function to create a new vector with a specific
 * capacity and size.
 * @param elem_size The size of an element of the vector.
 * @param args The capacity and allocator for the vector.
 * @param size The number of elements the vec must be initialised with.
 * @return The new vector.
 */
void *internal_vec_new(size_t elem_size, VecArgs args, size_t size);

/**
 * @brief Internal function to insert an element into the vector at the
 * specified index.
 * @param vector The vector.
 * @param elem A pointer to the element to insert.
 * @param index The index at which to insert the element.
 * @return The vector with the inserted element.
 */
void *internal_vec_insert(void *vector, void *elem, size_t index);

/**
 * @brief Internal function to push an element to the back of the vector.
 * @param vector The vector.
 * @param elem A pointer to the element to push.
 * @return The vector with the pushed element.
 */
void *internal_vec_push_back(void *vector, void *elem);

/**
 * @brief Internal function to extend the vector by copying elements
 * from an array.
 * @param vector The vector.
 * @param array The array to extend the vector from.
 * @param size The size of the array.
 * @return The vector with the extended elements.
 */
void *internal_vec_extend(void *vector, void *array, size_t size);

/**
 * @brief Creates a slice of the vector.
 * @param vector The vector.
 * @param buffer The buffer to which the elements are copied to.
 * @param args The start and end indices of the slice.
 * @return The size of the slice.
 */
size_t internal_vec_slice(void *vector, void *buffer, VecSliceArgs args);


#endif // VECTOR_H
