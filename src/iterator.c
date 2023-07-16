#include "../iterator.h"

static Option default_advance(Iterator *iterator, size_t n);
static size_t default_size(Iterator *iterator);

Iterator iter_default(void *container, void *current,
                          Option (*next)(Iterator *iterator)) {
    Iterator iterator = {
        .container = container,
        .current = current,
        .next = next,
        .advance = default_advance,
        .size = default_size
    };
    return iterator;
}

static Option default_advance(Iterator *iterator, size_t n) {
    Option ret = iterator->next(iterator);
    while (--n > 0 && iterator->next(iterator).is_valid);
    return ret;
}

static size_t default_size(Iterator *iterator) {
    size_t size = 0;
    for (; iter_next(*iterator).is_valid; ++size);
    return size;
}
