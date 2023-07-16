/**
 * @file option.h
 * @brief Definition and functions for working with optional values (Option type).
 */

#ifndef OPTION_H
#define OPTION_H

#include "base.h"

/**
 * @struct Option
 * @brief Represents an optional value.
 */
typedef struct {
    /** Pointer to the value contained in the option */
    void *value;

    /** Indicates whether the option contains a non null value */
    bool is_valid;
} Option;

/**
 * @typedef Option (*map_opt_fn)(void *)
 * @brief Function pointer for mapping a value to an Option.
 */
typedef Option (*map_opt_fn)(void *);

/**
 * @brief Creates an Option that wraps a NULL ptr.
 * @return A None Option.
 */
Option option_none();

/**
 * @brief Creates a an Option that wraps a non NULL ptr.
 * @param value A ptr to value to be contained in the Option.
 * @return A Some Option object containing the value.
 */
Option option_some(void *value);

/**
 * @brief Performs a logical OR operation between two options.
 * @param option1 The first Option.
 * @param option2 The second Option.
 * @return The Option resulting from the logical OR operation.
 */
Option option_or(Option option1, Option option2);


/**
 * @brief Performs a logical AND operation between two options.
 * @param option1 The first Option.
 * @param option2 The second Option.
 * @return The Option resulting from the logical AND operation.
 */
Option option_and(Option option1, Option option2);

/**
 * @brief Applies a function to the value of a Option if it is valid
 * and returns the resulting Option.
 * @param option The Option to be processed.
 * @param f The function to be applied to the value.
 * @return The Option resulting from applying the function.
 */
Option option_and_then(Option option, map_opt_fn f);

/**
 * @brief Applies a function to the value of an Option (no return value).
 * @param option The Option to be processed.
 * @param f The function to be applied to the value.
 */
void option_map(Option option, map_fn f);

/**
 * @brief Filters an Option based on a predicate.
 * @param option The Option to be filtered.
 * @param p The predicate function to apply.
 * @return The filtered Option.
 */
Option option_filter(Option option, pred_fn p);

/**
 * @brief Extracts the inner option from a nested Option.
 * @param option The nested Option.
 * @return The flattened Option.
 */
Option option_flatten(Option option);

/**
 * @brief Safely unwraps the value from a valid Option.
 * @param option The Option to unwrap.
 * @param type The expected type of the value.
 * @return The unwrapped value.
 * @remark If the Option is None, an assertion error will be triggered.
 */
#define option_unwrap(option, type) ({                                         \
    Option _opt = option;                                                      \
    ASSERT(_opt.is_valid, "unwrap called on object of type None");             \
    *(type *) (_opt.value);                                                    \
})

/**
 * @brief Unwraps the value from a valid Option with a fallback default value.
 * @param option The Option to unwrap.
 * @param type The expected type of the value.
 * @param default_val The fallback default value.
 * @return The unwrapped value or the default value if the Option is None.
 */
#define option_unwrap_or(option, type, default_val) ({                         \
    Option _opt = option;                                                      \
    ((_opt.is_valid) ? (*(type *) (_opt.value)) : default_val);                \
})


#endif // OPTION_H
