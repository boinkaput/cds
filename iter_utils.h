#ifndef ITER_UTILS_H
#define ITER_UTILS_H

#include "iterator.h"

bool iter_all(Iterator *iterator, pred_fn p);

bool iter_any(Iterator *iterator, pred_fn p);

Option iter_find(Iterator *iterator, pred_fn p);

Option iter_find_map(Iterator *iterator, Option (*func)(void *));

int iter_find_index(Iterator *iterator, pred_fn p);

bool iter_is_partitioned(Iterator *iterator, pred_fn p);

bool iter_is_sorted(Iterator *iterator, bool (*comp)(void *, void *));

Option iter_last(Iterator *iterator);

void iter_reduce(Iterator *iterator, void (*func)(void *, void *),
                 void *init);

/*--------------------------------- ForEach ---------------------------------*/

#define FOR_EACH(type, variable, iterator, body) {                             \
    Iterator _it = iterator;                                                   \
    for (Option option = iter_next(_it); option.is_valid;                      \
         option = iter_next(_it)) {                                            \
        type variable = option_unwrap(option, type);                           \
        body                                                                   \
    }                                                                          \
}

#define FOR_EACH_STEP(type, variable, iterator, step, body) {                  \
    Iterator _it = iterator;                                                   \
    size_t st = step;                                                          \
    for (Option option = iter_advance(_it, st); option.is_valid;               \
         option = iter_advance(_it, st)) {                                     \
        type variable = option_unwrap(option, type);                           \
        body                                                                   \
    }                                                                          \
}

/*--------------------------------- IterMap ---------------------------------*/

typedef struct {
    Iterator *iterator;
    map_fn f;
} Map;

Map map_new(Iterator *iterator, map_fn f);

Iterator map_iter(Map *map);

/*------------------------------- IterFilter --------------------------------*/

typedef struct {
    Iterator *iterator;
    pred_fn p;
} Filter;

Filter filter_new(Iterator *iterator, pred_fn p);

Iterator filter_iter(Filter *filter);


#endif // ITER_UTILS_H
