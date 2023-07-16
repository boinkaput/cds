#include <string.h>

#include "../vector.h"
#include "../base.h"

typedef struct {
    size_t capacity;
    size_t size;
    size_t elem_size;
    Allocator alloc;
} VectorMeta;

#define VECTOR_META_PTR(vector) (((VectorMeta *) vector) - 1)
#define VECTOR_PTR(vector_meta) (vector_meta + 1)

static void *resize(VectorMeta **vector_meta_p, size_t new_capacity);
static size_t find_new_capacity(size_t current_capacity, size_t required_capacity);
static Option vit_next(Iterator *iterator);
static Option vit_advance(Iterator *iterator, size_t n);
static size_t vit_size(Iterator *iterator);

void *internal_vec_new(size_t elem_size, Allocator alloc) {
    return internal_vec_with_cap(0, elem_size, alloc);
}

void *internal_vec_with_cap(size_t capacity, size_t elem_size,
                            Allocator alloc) {
    VectorMeta *vector_meta =
        allocator_allocate(alloc, sizeof(VectorMeta) + (elem_size * capacity));
    ASSERT(vector_meta != NULL, "Out of memory");
    vector_meta->capacity = capacity;
    vector_meta->size = 0;
    vector_meta->elem_size = elem_size;
    vector_meta->alloc = alloc;
    void *vector = VECTOR_PTR(vector_meta);
    memset(vector, 0, elem_size * capacity);
    return vector;
}

void *internal_vec_from_arr(void *array, size_t array_size, size_t elem_size,
                            Allocator alloc) {
    void *vector = internal_vec_with_cap(array_size, elem_size, alloc);
    memcpy(vector, array, elem_size * array_size);
    VECTOR_META_PTR(vector)->size = array_size;
    return vector;
}

size_t vec_size(void *vector) {
    return VECTOR_META_PTR(vector)->size;
}

size_t vec_capacity(void *vector) {
    return VECTOR_META_PTR(vector)->capacity;
}

bool vec_is_empty(void *vector) {
    return vec_size(vector) == 0;
}

void *internal_vec_insert(void *vector, void *elem, size_t index) {
    VectorMeta *vector_meta = VECTOR_META_PTR(vector);
    ASSERT(index <= vector_meta->size, "Index (is %zu) should be <= vector_size (is %zu)",
           index, vector_meta->size);
    vector = resize(&vector_meta, find_new_capacity(vector_meta->capacity,
                                                    vector_meta->size + 1));
    ASSERT(vector != NULL, "Out of memory");
    memmove(vector + ((index + 1) * vector_meta->elem_size),
            vector + (index * vector_meta->elem_size),
            ((vector_meta->size++) - index) * vector_meta->elem_size);
    memmove(vector + (index * vector_meta->elem_size),
            elem, vector_meta->elem_size);
    return vector;
}

void *internal_vec_push_back(void *vector, void *elem) {
    VectorMeta *vector_meta = VECTOR_META_PTR(vector);
    vector = resize(&vector_meta, find_new_capacity(vector_meta->capacity,
                                                    vector_meta->size + 1));
    ASSERT(vector != NULL, "Out of memory");
    memmove(vector + (vector_meta->size++ * vector_meta->elem_size),
            elem, vector_meta->elem_size);
    return vector;
}

void *internal_vec_extend(void *vector, void *array, size_t array_size) {
    VectorMeta *vector_meta = VECTOR_META_PTR(vector);
    size_t total_size = vector_meta->size + array_size;
    vector = resize(&vector_meta, find_new_capacity(vector_meta->capacity,
                                                    total_size));
    ASSERT(vector != NULL, "Out of memory");
    memmove(vector + (vector_meta->size * vector_meta->elem_size),
            array, array_size * vector_meta->elem_size);
    vector_meta->size += array_size;
    return vector;
}

void vec_erase(void *vector, size_t index, void *elem) {
    VectorMeta *vector_meta = VECTOR_META_PTR(vector);
    ASSERT(index < vector_meta->size, "Index (is %zu) should be < vector_size (is %zu)",
           index, vector_meta->size);
    if (elem != NULL) {
        memmove(elem, vector + (index * vector_meta->elem_size),
                vector_meta->elem_size);
    }
    memmove(vector + (index * vector_meta->elem_size),
            vector + ((index + 1) * vector_meta->elem_size),
            ((--vector_meta->size) - index) * vector_meta->elem_size);
}

void vec_swap_erase(void *vector, size_t index, void *elem) {
    VectorMeta *vector_meta = VECTOR_META_PTR(vector);
    ASSERT(index < vector_meta->size, "Index (is %zu) should be < vector_size (is %zu)",
           index, vector_meta->size);
    if (elem != NULL) {
        memmove(elem, vector + (index * vector_meta->elem_size),
                vector_meta->elem_size);
    }
    if (index != --vector_meta->size) {
        memmove(vector + (index * vector_meta->elem_size),
                vector + (vector_meta->size * vector_meta->elem_size),
                vector_meta->elem_size);
    }
}

void vec_pop_back(void *vector, void *elem) {
    VectorMeta *vector_meta = VECTOR_META_PTR(vector);
    ASSERT(vector_meta->size > 0, "vector_size (is %zu) should be > 0",
           vector_meta->size);
    --vector_meta->size;
    if (elem != NULL) {
        memmove(elem, vector + (vector_meta->size * vector_meta->elem_size),
                vector_meta->elem_size);
    }
}

void vec_clear(void *vector) {
    VECTOR_META_PTR(vector)->size = 0;
}

void vec_free(void *vector) {
    VectorMeta *vector_meta = VECTOR_META_PTR(vector);
    allocator_deallocate(vector_meta->alloc, vector_meta);
}

void *vec_reserve(void *vector, size_t new_capacity) {
    VectorMeta *vector_meta = VECTOR_META_PTR(vector);
    if (new_capacity > vector_meta->capacity) {
        vector = resize(&vector_meta, new_capacity);
        ASSERT(vector != NULL, "Out of memory");
    }
    return vector;
}

void *vec_shrink(void *vector) {
    VectorMeta *vector_meta = VECTOR_META_PTR(vector);
    vector = resize(&vector_meta, vector_meta->size);
    ASSERT(vector != NULL, "Out of memory");
    return vector;
}

size_t vec_slice(void *vector, size_t start, size_t end, void *buffer) {
    VectorMeta *vector_meta = VECTOR_META_PTR(vector);
    ASSERT(buffer != NULL, "buffer should not be NULL");
    ASSERT(start <= end, "start (is %zu) should be <= end (is %zu)", start, end);
    ASSERT(end <= vector_meta->size, "end (is %zu) should be <= vector_size (is %zu)",
           end, vector_meta->size);
    memmove(buffer, vector + (start * vector_meta->elem_size),
            (end - start) * vector_meta->elem_size);
    return end - start;
}

Iterator vec_iter(void *vector) {
    Iterator iterator = iter_default(vector, vector, vit_next);
    iterator.advance = vit_advance;
    iterator.size = vit_size;
    return iterator;
}

static void *resize(VectorMeta **vector_meta_p, size_t new_capacity) {
    if (new_capacity != (*vector_meta_p)->capacity) {
        (*vector_meta_p)->capacity = new_capacity;
        VectorMeta *vector_meta = allocator_reallocate(
            (*vector_meta_p)->alloc, *vector_meta_p,
            sizeof(VectorMeta) + ((*vector_meta_p)->elem_size * new_capacity));
        if (vector_meta == NULL) {
            return NULL;
        }
        *vector_meta_p = vector_meta;
    }
    return VECTOR_PTR(*vector_meta_p);
}

static size_t find_new_capacity(size_t current_capacity, size_t required_capacity) {
    current_capacity = (current_capacity == 0) ? 1 : current_capacity;
    while (current_capacity < required_capacity) {
        current_capacity *= 2;
    }
    return current_capacity;
}

static Option vit_next(Iterator *iterator) {
    VectorMeta *vector_meta = VECTOR_META_PTR(iterator->container);
    if (iterator->current >=
        iterator->container + (vector_meta->size * vector_meta->elem_size)) {
        return option_none();
    } else {
        void *ret = iterator->current;
        iterator->current += vector_meta->elem_size;
        return option_some(ret);
    }
}

static Option vit_advance(Iterator *iterator, size_t n) {
    VectorMeta *vector_meta = VECTOR_META_PTR(iterator->container);
    if (iterator->current >=
        iterator->container + (vector_meta->size * vector_meta->elem_size)) {
        return option_none();
    } else {
        void *ret = iterator->current;
        iterator->current += (n * vector_meta->elem_size);
        return option_some(ret);
    }
}

static size_t vit_size(Iterator *iterator) {
    VectorMeta *vector_meta = VECTOR_META_PTR(iterator->container);
    void *vector_end = iterator->container + (vector_meta->size * vector_meta->elem_size);
    size_t size = (vector_end - iterator->current) / vector_meta->elem_size;
    iterator->current = vector_end;
    return size;
}
