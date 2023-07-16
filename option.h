#ifndef OPTION_H
#define OPTION_H

#include "base.h"

typedef struct {
    void *value;
    bool is_valid;
} Option;

Option option_none();

Option option_some(void *value);

Option option_or(Option option1, Option option2);

Option option_and(Option option1, Option option2);

Option option_and_then(Option option, Option (*fn)(void *));

void option_map(Option option, map_fn f);

Option option_filter(Option option, pred_fn p);

Option option_flatten(Option option);

#define option_unwrap(option, type) ({                                         \
    Option _opt = option;                                                      \
    ASSERT(_opt.is_valid, "unwrap called on object of type None");             \
    *(type *) (_opt.value);                                                    \
})

#define option_unwrap_or(option, type, default_val) ({                         \
    Option _opt = option;                                                      \
    ((_opt.is_valid) ? (*(type *) (_opt.value)) : default_val);                \
})


#endif // OPTION_H
