#include "../iter_utils.h"

static Option mit_next(Iterator *iterator);
static Option mit_advance(Iterator *iterator, size_t n);
static Option fit_next(Iterator *iterator);
static Option fit_advance(Iterator *iterator, size_t n);

bool iter_all(Iterator *iterator, pred_fn p) {
    for (Option option = iter_next(*iterator); option.is_valid;
         option = iter_next(*iterator)) {
        if (!option_filter(option, p).is_valid) {
            return false;
        }
    }
    return true;
}

bool iter_any(Iterator *iterator, pred_fn p) {
    for (Option option = iter_next(*iterator); option.is_valid;
         option = iter_next(*iterator)) {
        if (option_filter(option, p).is_valid) {
            return true;
        }
    }
    return false;
}

Option iter_find(Iterator *iterator, pred_fn p) {
    for (Option option = iter_next(*iterator); option.is_valid;
         option = iter_next(*iterator)) {
        if (option_filter(option, p).is_valid) {
            return option;
        }
    }
    return option_none();
}

Option iter_find_map(Iterator *iterator, Option (*func)(void *)) {
    for (Option option = iter_next(*iterator); option.is_valid;
         option = iter_next(*iterator)) {
        option = option_and_then(option, func);
        if (option.is_valid) {
            return option;
        }
    }
    return option_none();
}

int iter_find_index(Iterator *iterator, pred_fn p) {
    size_t index = 0;
    for (Option option = iter_next(*iterator); option.is_valid;
         option = iter_next(*iterator), ++index) {
        if (option_filter(option, p).is_valid) {
            return index;
        }
    }
    return -1;
}

bool iter_is_partitioned(Iterator *iterator, pred_fn p) {
    return iter_all(iterator, p) || !iter_any(iterator, p);
}

bool iter_is_sorted(Iterator *iterator, bool (*comp)(void *, void *)) {
    for (Option option_prev = iter_next(*iterator), option = iter_next(*iterator);
         option.is_valid; option_prev = option, option = iter_next(*iterator)) {
        if (!comp(option_prev.value, option.value)) {
            return false;
        }
    }
    return true;
}

bool iter_compare(Iterator *iterator1, Iterator *iterator2,
                  bool (*comp)(void *, void *)) {
    Option option1 = iter_next(*iterator1);
    Option option2 = iter_next(*iterator2);
    for (; option_and(option1, option2).is_valid;
         option1 = iter_next(*iterator1), option2 = iter_next(*iterator2)) {
        if (!comp(option1.value, option2.value)) {
            return false;
        }
    }
    return !option_or(option1, option2).is_valid;
}

Option iter_last(Iterator *iterator) {
    Option option_prev = iter_next(*iterator);
    for (Option option = iter_next(*iterator); option.is_valid;
         option_prev = option, option = iter_next(*iterator));
    return option_prev;
}

void iter_reduce(Iterator *iterator, void (*func)(void *, void *), void *init) {
    for (Option option = iter_next(*iterator); option.is_valid;
         option = iter_next(*iterator)) {
        func(init, option.value);
    }
}

Map map_new(Iterator *iterator, map_fn f) {
    return (Map) {
        .iterator = iterator,
        .f = f
    };
}

Iterator map_iter(Map *map) {
    Iterator iterator = iter_default(NULL, map, mit_next);
    iterator.advance = mit_advance;
    iterator.size = map->iterator->size;
    return iterator;
}

Filter filter_new(Iterator *iterator, pred_fn p) {
    return (Filter) {
        .iterator = iterator,
        .p = p
    };
}

Iterator filter_iter(Filter *filter) {
    Iterator iterator = iter_default(NULL, filter, fit_next);
    iterator.advance = fit_advance;
    iterator.size = filter->iterator->size;
    return iterator;
}

static Option mit_next(Iterator *iterator) {
    Map *current = iterator->current;
    Option option = iter_next(*(current->iterator));
    option_map(option, current->f);
    return option;
}

static Option mit_advance(Iterator *iterator, size_t n) {
    Map *current = iterator->current;
    Option option = iter_advance(*(current->iterator), n);
    option_map(option, current->f);
    return option;
}

static Option fit_next(Iterator *iterator) {
    Filter *current = iterator->current;
    return iter_find(current->iterator, current->p);
}

static Option fit_advance(Iterator *iterator, size_t n) {
    Filter *current = iterator->current;
    iter_advance(*(current->iterator), n);
    return iter_find(iterator, current->p);
}
