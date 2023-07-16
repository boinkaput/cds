#include "../slice.h"

static Option sit_next(Iterator *iterator);
static Option sit_advance(Iterator *iterator, size_t n);
static size_t sit_size(Iterator *iterator);

Iterator slice_iter(Slice *slice) {
    Iterator iterator = iter_default(slice, slice->array, sit_next);
    iterator.advance = sit_advance;
    iterator.size = sit_size;
    return iterator;
}

static Option sit_next(Iterator *iterator) {
    Slice *slice = iterator->container;
    if (iterator->current >= slice->array + (slice->size * slice->elem_size)) {
        return option_none();
    } else {
        void *ret = iterator->current;
        iterator->current += slice->elem_size;
        return option_some(ret);
    }
}

static Option sit_advance(Iterator *iterator, size_t n) {
    Slice *slice = iterator->container;
    if (iterator->current >= slice->array + (slice->size * slice->elem_size)) {
        return option_none();
    } else {
        void *ret = iterator->current;
        iterator->current += (n * slice->elem_size);
        return option_some(ret);
    }
}

static size_t sit_size(Iterator *iterator) {
    Slice *slice = iterator->container;
    void *slice_end = slice->array + (slice->size * slice->elem_size);
    size_t size = (slice_end - iterator->current) / slice->elem_size;
    iterator->current = slice_end;
    return size;
}
