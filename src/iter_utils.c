#include "../iter_utils.h"

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

Option iter_last(Iterator *iterator) {
    Option option_prev = iter_next(*iterator);
    for (Option option = iter_next(*iterator); option.is_valid;
         option_prev = option, option = iter_next(*iterator));
    return option_prev;
}

void iter_fold(Iterator *iterator, void (*func)(void *, void *), void *init) {
    for (Option option = iter_next(*iterator); option.is_valid;
         option = iter_next(*iterator)) {
        func(init, option.value);
    }
}

void iter_reduce(Iterator *iterator, void (*func)(void *, void *), Option *option) {
    *option = iter_next(*iterator);
    if (option->is_valid) {
        iter_fold(iterator, func, option->value);
    }
}
