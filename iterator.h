#ifndef ITERATOR_H
#define ITERATOR_H

#include <stddef.h>

#include "option.h"

typedef struct iterator {
    void *container;
    void *current;
    Option (*next)(struct iterator *iterator);
    Option (*advance)(struct iterator *iterator, size_t n);
    size_t (*size)(struct iterator *iterator);
} Iterator;

Iterator iter_default(
    void *container,
    void *current,
    Option (*next)(Iterator *iterator)
);


#define iter_next(iterator)                                \
    ((iterator).next(&(iterator)))

#define iter_advance(iterator, n)                          \
    ((iterator).advance(&(iterator), n))

#define iter_size(iterator)                                \
    ((iterator).size(&(iterator)))

#define iter_get(iterator, index) (                        \
    iter_advance(&(iterator), index);                      \
    iter_next(&(iterator));                                \
)


#endif // ITERATOR_H
