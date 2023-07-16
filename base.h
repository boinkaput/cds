#ifndef ASSERT_H
#define ASSERT_H

#include <stdbool.h>

#ifndef NDEBUG
#include <stdio.h>
#include <stdlib.h>
#define ASSERT(condition, err_msg, ...)                                           \
    do {                                                                          \
        if (!(condition)) {                                                       \
            fprintf(stderr, "ERROR: %s: " err_msg "\n", __func__, ##__VA_ARGS__); \
            exit(EXIT_FAILURE);                                                   \
        }                                                                         \
    } while(0)
#else
#define ASSERT(condition, err_msg, ...)
#endif // NDEBUG

typedef void (*map_fn)(void *);
typedef bool (*pred_fn)(void *);

#endif // ASSERT_H
