#include "../iter_utils.h"

static Option mit_next(Iterator *iterator);
static Option mit_advance(Iterator *iterator, size_t n);
static Option fit_next(Iterator *iterator);
static Option fit_advance(Iterator *iterator, size_t n);

bool iter_all(Iterator *iterator, pred_fn predicate) {
    for (
        Option option = iter_next(*iterator);
        option.is_valid;
        option = iter_next(*iterator)
    ) {
        if (!option_filter(option, predicate).is_valid) {
            return false;
        }
    }
    return true;
}

bool iter_any(Iterator *iterator, pred_fn predicate) {
    for (
        Option option = iter_next(*iterator);
        option.is_valid;
        option = iter_next(*iterator)
    ) {
        if (option_filter(option, predicate).is_valid) {
            return true;
        }
    }
    return false;
}

Option iter_find(Iterator *iterator, pred_fn predicate) {
    for (
        Option option = iter_next(*iterator);
        option.is_valid;
        option = iter_next(*iterator)
    ) {
        if (option_filter(option, predicate).is_valid) {
            return option;
        }
    }
    return option_none();
}

Option iter_find_map(Iterator *iterator, map_opt_fn unary_op) {
    for (
        Option option = iter_next(*iterator);
        option.is_valid;
        option = iter_next(*iterator)
    ) {
        option = option_and_then(option, unary_op);
        if (option.is_valid) {
            return option;
        }
    }
    return option_none();
}

int iter_find_index(Iterator *iterator, pred_fn predicate) {
    int index = 0;
    for (
        Option option = iter_next(*iterator);
        option.is_valid;
        option = iter_next(*iterator)
    ) {
        if (option_filter(option, predicate).is_valid) {
            return index;
        }
        index++;
    }
    return -1;
}

bool iter_is_partitioned(Iterator *iterator, pred_fn predicate) {
    return iter_all(iterator, predicate) || !iter_any(iterator, predicate);
}

bool iter_is_sorted(Iterator *iterator, compare_fn compare) {
    for (
        Option option_prev = iter_next(*iterator), option_cur = iter_next(*iterator);
        option_cur.is_valid;
        option_prev = option_cur, option_cur = iter_next(*iterator)
    ) {
        if (compare(option_prev.value, option_cur.value) > 0) {
            return false;
        }
    }
    return true;
}

int iter_compare(Iterator *iterator1, Iterator *iterator2, compare_fn compare) {
    Option option1 = iter_next(*iterator1);
    Option option2 = iter_next(*iterator2);
    for (
        ;
        option_and(option1, option2).is_valid;
        option1 = iter_next(*iterator1), option2 = iter_next(*iterator2)
    ) {
        int cmp_result = compare(option1.value, option2.value);
        if (cmp_result != 0) {
            return cmp_result;
        }
    }
    return (option1.is_valid) ? 1 : ((option2.is_valid) ? -1 : 0);
}

Option iter_last(Iterator *iterator) {
    Option option_prev = iter_next(*iterator);
    for (
        Option option_cur = iter_next(*iterator);
        option_cur.is_valid;
        option_prev = option_cur, option_cur = iter_next(*iterator)
    );
    return option_prev;
}

void iter_reduce(Iterator *iterator, void (*func)(void *, void *), void *init) {
    for (Option option = iter_next(*iterator); option.is_valid;
         option = iter_next(*iterator)) {
        func(init, option.value);
    }
}

Iterator map_iter(Map *map, Iterator *iterator, map_fn unary_op) {
    map->iterator = iterator;
    map->unary_op = unary_op;
    Iterator map_iterator = iter_default(NULL, map, mit_next);
    map_iterator.advance = mit_advance;
    map_iterator.size = map->iterator->size;
    return map_iterator;
}

Iterator filter_iter(Filter *filter, Iterator *iterator, pred_fn predicate) {
    filter->iterator = iterator;
    filter->predicate = predicate;
    Iterator filter_iterator = iter_default(NULL, filter, fit_next);
    filter_iterator.advance = fit_advance;
    return filter_iterator;
}

static Option mit_next(Iterator *iterator) {
    Map *current = iterator->current;
    Option option = iter_next(*(current->iterator));
    option_map(option, current->unary_op);
    return option;
}

static Option mit_advance(Iterator *iterator, size_t n) {
    Map *current = iterator->current;
    Option option = iter_advance(*(current->iterator), n);
    option_map(option, current->unary_op);
    return option;
}

static Option fit_next(Iterator *iterator) {
    Filter *current = iterator->current;
    return iter_find(current->iterator, current->predicate);
}

static Option fit_advance(Iterator *iterator, size_t n) {
    Filter *current = iterator->current;
    iter_advance(*(current->iterator), n);
    return iter_find(iterator, current->predicate);
}
