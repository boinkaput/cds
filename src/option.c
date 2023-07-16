#include "../option.h"

#include <stddef.h>

Option option_none() {
    return (Option) {
        .value = NULL,
        .is_valid = false
    };
}

Option option_some(void *value) {
    return (Option) {
        .value = value,
        .is_valid = true
    };
}

Option option_or(Option option1, Option option2) {
    return option1.is_valid ? option1 : option2;
}

Option option_and(Option option1, Option option2) {
    return option1.is_valid ? option2 : option_none();
}

Option option_and_then(Option option, Option (*fn)(void *)) {
    if (option.is_valid) {
        return fn(option.value);
    } else {
        return option_none();
    }
}

void option_map(Option option, map_fn f) {
    if (option.is_valid) {
        f(option.value);
    }
}

Option option_filter(Option option, pred_fn p) {
    return option.is_valid && p(option.value) ? option : option_none();
}

Option option_flatten(Option option) {
    return option.is_valid ? *((Option *) option.value) : option_none();
}
