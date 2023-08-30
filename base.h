/**
 * @file base.h
 * @brief Defines an assertion mechanism and function pointer types for
 * mapping and predicate functions.
 */

#ifndef BASE_H
#define BASE_H

#include <stdbool.h>

#ifndef NDEBUG

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Asserts a condition and terminates the program if the condition is false.
 * @param condition The condition to check.
 * @param err_msg The error message format string (supports variadic arguments).
 * @remark If the condition is false, the function prints the error message to stderr
 * with additional optional arguments using the provided format, then terminates the
 * program with EXIT_FAILURE.
 */
#define ASSERT(condition, err_msg, ...)                                           \
    do {                                                                          \
        if (!(condition)) {                                                       \
            fprintf(stderr, "ERROR: %s: " err_msg "\n", __func__, ##__VA_ARGS__); \
            exit(EXIT_FAILURE);                                                   \
        }                                                                         \
    } while(0)

#else // ifdef NDEBUG

/**
 * @brief Does nothing.
 * @param condition The condition to check.
 * @param err_msg The error message format string (supports variadic arguments).
 * @remark Does nothing enabled when NDEBUG is defined.
 */
#define ASSERT(condition, err_msg, ...)

#endif // NDEBUG

/**
 * @brief Function pointer type for mapping functions.
 * @param value The data to process.
 */
typedef void (*map_fn)(void *value);

/**
 * @brief Function pointer type for predicate functions.
 * @param value The value to test against a condition.
 * @return true if the condition is satisfied, false otherwise.
 */
typedef bool (*pred_fn)(const void *value);

/**
 * @brief Function pointer type for comparator functions.
 * @param value1 The first value.
 * @param value1 The second value.
 * @return 0 if `value1 == value2`, negative value if `value1 < value2`
 * and positive value if `value1 > value2`
 */
typedef int (*compare_fn)(const void *value1, const void *value2);


#endif // BASE_H
