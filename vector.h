/**
 * @file vector.h
 * @brief Definition and functions for a dynamic array container.
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

#include "allocator.h"
#include "iterator.h"
#include "slice.h"

/**
 * @brief Macro to define a vector type with the specified element type.
 * @param elem_type The type of the elements in the vector.
 * @return The defined vector type.
 */
#define Vec(elem_type) elem_type *

/**
 * @brief Define VECTOR_ALLOCATOR to gain access to the functions that allow
 * a custom allocator to be supplied.
*/
#ifndef VECTOR_ALLOCATOR

/**
 * @brief Creates a new vector with the specified element type.
 * @param elem_type The type of the elements in the vector.
 * @return The created vector.
 */
#define vec_new(elem_type)                                                     \
    internal_vec_new(sizeof(elem_type), allocator_new())

/**
 * @brief Creates a new vector with the specified element type and capacity.
 * @param elem_type The type of the elements in the vector.
 * @param capacity The initial capacity of the vector.
 * @return The created vector.
 */
#define vec_with_capacity(elem_type, capacity)                                 \
    internal_vec_with_cap(capacity, sizeof(elem_type), allocator_new())

/**
 * @brief Creates a new vector from a slice.
 * @param slice The slice containing elements to copy.
 * @return The created vector.
 */
#define vec_from_slice(slice)                                                  \
    internal_vec_from_slice(slice, allocator_new())

#else

/**
 * @brief Creates a new vector with the specified element type and
 * custom allocator.
 * @param elem_type The type of the elements in the vector.
 * @param allocator The custom allocator for memory allocation.
 * @return The created vector.
 */
#define vec_new(elem_type, allocator)                                          \
    internal_vec_new(sizeof(elem_type), allocator)

/**
 * @brief Creates a new vector with the specified element type, capacity,
 * and custom allocator.
 * @param elem_type The type of the elements in the vector.
 * @param capacity The initial capacity of the vector.
 * @param allocator The custom allocator for memory allocation.
 * @return The created vector.
 */
#define vec_with_capacity(elem_type, capacity, allocator)                      \
    internal_vec_with_cap(sizeof(elem_type), capacity, allocator)

/**
 * @brief Creates a new vector from a slice with a custom allocator.
 * @param slice The slice containing elements to copy.
 * @param allocator The custom allocator for memory allocation.
 * @return The created vector.
 */
#define vec_from_slice(slice, allocator)                                       \
    internal_vec_from_slice(slice, allocator)

#endif // VECTOR_ALLOCATOR

/**
 * @brief Inserts an element into the vector at the specified index.
 * @param vector The vector.
 * @param elem The element to insert.
 * @param index The index at which to insert the element.
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
 */
#define vec_push_back(vector, elem)                                            \
    do {                                                                       \
        typeof(*vector) e = elem;                                              \
        vector = internal_vec_push_back(vector, &e);                           \
    } while(0)

/**
 * @brief Extends the vector by copying elements from a slice.
 * @param vector The vector.
 * @param slice The slice to extend the vector from.
 */
#define vec_extend(vector, slice)                                              \
    do {                                                                       \
        vector = internal_vec_extend(vector, slice);                           \
    } while (0)

/**
 * @brief Returns the size (number of elements) of the vector.
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
 * @return true if the vector is empty, false otherwise.
 */
bool vec_is_empty(void *vector);

/**
 * @brief Erases an element at the specified index in the vector.
 * @param vector The vector.
 * @param index The index of the element to erase.
 * @param elem A pointer to store the erased element.
 * Supply NULL if you don't care about the value.
 */
void vec_erase(void *vector, size_t index, void *elem);

/**
 * @brief Erases by swapping the element at the specified index with the
 * last element in the vector.
 * @param vector The vector.
 * @param index The index of the element to erase.
 * @param elem A pointer to store the erased element.
 * Supply NULL if you don't care about the value.
 */
void vec_swap_erase(void *vector, size_t index, void *elem);

/**
 * @brief Removes the last element from the vector.
 * @param vector The vector.
 * @param elem A pointer to store the popped element.
 * Supply NULL if you don't care about the value.
 */
void vec_pop_back(void *vector, void *elem);

/**
 * @brief Clears the vector, removing all elements.
 * @param vector The vector to clear.
 */
void vec_clear(void *vector);

/**
 * @brief Frees the memory used by the vector.
 * @param vector The vector to free.
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
 * @brief Creates a slice of the vector.
 * @param vector The vector.
 * @param start The start index of the slice.
 * @param end The end index of the slice.
 * @return The slice spanning from start to end of the vector.
 */
Slice vec_slice(void *vector, size_t start, size_t end);

/**
 * @brief Creates an iterator for the vector.
 * @param vector The vector.
 * @return An iterator for the vector.
 */
Iterator vec_iter(void *vector);

/*-------------------------- Functions used by API --------------------------*/

/**
 * @brief Internal function to create a new vector.
 * @param elem_size The size of each element in the vector.
 * @param alloc The allocator for memory allocation.
 * @return A pointer to the new vector.
 */
void *internal_vec_new(size_t elem_size, Allocator alloc);

/**
 * @brief Internal function to create a new vector with a specific capacity.
 * @param capacity The initial capacity of the vector.
 * @param elem_size The size of each element in the vector.
 * @param alloc The allocator for memory allocation.
 * @return A pointer to the new vector.
 */
void *internal_vec_with_cap(size_t capacity, size_t elem_size, Allocator alloc);

/**
 * @brief Internal function to create a new vector from a Slice.
 * @param Slice The slice to create the vector from.
 * @param alloc The allocator for memory allocation.
 * @return A pointer to the new vector.
 */
void *internal_vec_from_slice(Slice slice, Allocator alloc);

/**
 * @brief Internal function to insert an element into the vector at the specified index.
 * @param vector The vector.
 * @param elem A pointer to the element to insert.
 * @param index The index at which to insert the element.
 * @return A pointer to the vector with the inserted element.
 */
void *internal_vec_insert(void *vector, void *elem, size_t index);

/**
 * @brief Internal function to push an element to the back of the vector.
 * @param vector The vector.
 * @param elem A pointer to the element to push.
 * @return A pointer to the vector with the pushed element.
 */
void *internal_vec_push_back(void *vector, void *elem);

/**
 * @brief Internal function to extend the vector by copying elements from a slice.
 * @param vector The vector.
 * @param slice The slice to extend the vector from.
 * @return A pointer to the vector with the extended elements.
 */
void *internal_vec_extend(void *vector, Slice slice);


#endif // VECTOR_H
