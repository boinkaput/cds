#ifndef ITER_UTILS_H
#define ITER_UTILS_H

#include "iterator.h"

#define FOR_EACH(type, variable, iterator, body) {                             \
    Iterator it = iterator;                                                    \
    for (Option option = iter_next(it); option.is_valid;                       \
         option = iter_next(it)) {                                             \
        type variable = option_unwrap(option, type);                           \
        body                                                                   \
    }                                                                          \
}

#define FOR_EACH_STEP(type, variable, iterator, step, body) {                  \
    Iterator it = iterator;                                                    \
    size_t st = step;                                                          \
    for (Option option = iter_advance(it, st); option.is_valid;                \
         option = iter_advance(it, st)) {                                      \
        type variable = option_unwrap(option, type);                           \
        body                                                                   \
    }                                                                          \
}

bool iter_all(Iterator *iterator, pred_fn p);

bool iter_any(Iterator *iterator, pred_fn p);

Option iter_find(Iterator *iterator, pred_fn p);

Option iter_find_map(Iterator *iterator, Option (*func)(void *));

int iter_find_index(Iterator *iterator, pred_fn p);

bool iter_is_partitioned(Iterator *iterator, pred_fn p);

bool iter_is_sorted(Iterator *iterator, bool (*comp)(void *, void *));

Option iter_last(Iterator *iterator);

void iter_fold(Iterator *iterator, void (*func)(void *, void *),
               void *init);

void iter_reduce(Iterator *iterator, void (*func)(void *, void *),
                 Option *option);


#endif // ITER_UTILS_H
