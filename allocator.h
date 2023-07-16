/**
 * @file allocator.h
 * @brief Interface and functions for an allocator.
 */

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

/**
 * @struct Allocator
 * @brief Represents an allocator.
 */
typedef struct allocator {
    /** Context data for the allocator */
    void *ctx;

    /** Function for allocating memory */
    void *(*allocate)(struct allocator alloc, size_t size);

    /** Function for reallocating memory */
    void *(*reallocate)(struct allocator alloc, void *ptr, size_t size);

    /**< Function pointer for freeing memory */
    void (*deallocate)(struct allocator alloc, void *ptr);
} Allocator;

/**
 * @brief Allocates memory using the provided allocator.
 * @param alloc The allocator to use for memory allocation.
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block.
 */
#define allocator_allocate(alloc, size) \
    alloc.allocate(alloc, size)

/**
 * @brief Reallocates memory using the provided allocator.
 * @param alloc The allocator to use for memory reallocation.
 * @param ptr A pointer to the memory block to reallocate.
 * @param size The new size of the memory block.
 * @return A pointer to the reallocated memory block.
 */
#define allocator_reallocate(alloc, ptr, size) \
    alloc.reallocate(alloc, ptr, size)

/**
 * @brief Deallocates memory using the provided allocator.
 * @param alloc The allocator to use for memory deallocation.
 * @param ptr A pointer to the memory block to deallocate.
 */
#define allocator_deallocate(alloc, ptr) \
    alloc.deallocate(alloc, ptr)

/**
 * @brief Creates an Allocator with malloc, realloc and free.
 * @return The Allocator.
 */
Allocator allocator_new();


#endif // ALLOCATOR_H
