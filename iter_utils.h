#ifndef ITER_UTILS_H
#define ITER_UTILS_H

#include "iterator.h"

bool iter_all(Iterator *iterator, pred_fn predicate);

bool iter_any(Iterator *iterator, pred_fn predicate);

Option iter_find(Iterator *iterator, pred_fn predicate);

Option iter_find_map(Iterator *iterator, map_opt_fn unary_op);

int iter_find_index(Iterator *iterator, pred_fn predicate);

bool iter_is_partitioned(Iterator *iterator, pred_fn predicate);

bool iter_is_sorted(Iterator *iterator, compare_fn compare);

int iter_compare(Iterator *iterator1, Iterator *iterator2, compare_fn compare);

Option iter_last(Iterator *iterator);

void iter_reduce(Iterator *iterator, void (*func)(void *, void *),
                 void *init);

/*--------------------------------- ForEach ---------------------------------*/

#define for_each(type, variable, iterator, body)                               \
    do {                                                                       \
        Iterator _it = iterator;                                               \
        for (                                                                  \
            Option option = iter_next(_it);                                    \
            option.is_valid;                                                   \
            option = iter_next(_it)                                            \
        ) {                                                                    \
            type variable = option_unwrap(option, type);                       \
            body                                                               \
        }                                                                      \
    } while(0)

#define for_each_step(type, variable, iterator, step, body)                    \
    do {                                                                       \
        Iterator _it = iterator;                                               \
        size_t _st = step;                                                     \
        for (                                                                  \
            Option option = iter_advance(_it, _st);                            \
            option.is_valid;                                                   \
            option = iter_advance(_it, _st)                                    \
        ) {                                                                    \
            type variable = option_unwrap(option, type);                       \
            body                                                               \
        }                                                                      \
    } while (0)

/*--------------------------------- IterMap ---------------------------------*/

typedef struct {
    Iterator *iterator;
    map_fn unary_op;
} Map;

Iterator map_iter(Map *map, Iterator *iterator, map_fn unary_op);

/*------------------------------- IterFilter --------------------------------*/

typedef struct {
    Iterator *iterator;
    pred_fn predicate;
} Filter;

Iterator filter_iter(Filter *filter, Iterator *iterator, pred_fn predicate);


#endif // ITER_UTILS_H
