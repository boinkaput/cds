#include "../base.h"
#include "../vector.h"

#define VEC_META_PTR(vector) (((VectorMeta *) vector) - 1)
#define VEC_PTR(vector_meta) ((void *) (vector_meta + 1))
#define VEC_GET(vector, index, elem_size) (void *) ((size_t) vector + ((index) * elem_size))

typedef struct {
    size_t capacity;
    size_t size;
    size_t elem_size;
    Allocator alloc;
} VectorMeta;

static void *resize(VectorMeta **vector_meta_ref, size_t new_capacity);
static size_t find_new_capacity(size_t current_capacity, size_t required_capacity);
static void swap(void *ptr1, void *ptr2, size_t size);
static Option vit_next(Iterator *iterator);
static Option vit_advance(Iterator *iterator, size_t n);
static size_t vit_size(Iterator *iterator);

void *internal_vec_new(size_t elem_size, VecArgs args, size_t size) {
    args.cap = (size > args.cap) ? size : args.cap;
    VectorMeta *vector_meta = allocator_allocate(
        args.alloc,
        sizeof(VectorMeta) + (elem_size * args.cap)
    );
    ASSERT(vector_meta != NULL, "Out of memory");

    vector_meta->capacity = args.cap;
    vector_meta->size = size;
    vector_meta->elem_size = elem_size;
    vector_meta->alloc = args.alloc;

    // Initialise vec elements to 0.
    return memset(VEC_PTR(vector_meta), 0, elem_size * args.cap);
}

size_t vec_size(void *vector) {
    return VEC_META_PTR(vector)->size;
}

size_t vec_capacity(void *vector) {
    return VEC_META_PTR(vector)->capacity;
}

bool vec_is_empty(void *vector) {
    return vec_size(vector) == 0;
}

void *internal_vec_insert(void *vector, void *elem, size_t index) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    ASSERT(
        index <= vector_meta->size,
        "Index (is %zu) should be <= vector_size (is %zu)",
        index, vector_meta->size
    );

    vector = resize(
        &vector_meta,
        find_new_capacity(vector_meta->capacity, vector_meta->size + 1)
    );
    ASSERT(vector != NULL, "Out of memory");

    // Push all elements after index to the right by 1 position.
    memmove(
        VEC_GET(vector, index + 1, vector_meta->elem_size),
        VEC_GET(vector, index, vector_meta->elem_size),
        (vector_meta->size - index) * vector_meta->elem_size
    );

    // Insert the element.
    memcpy(
        VEC_GET(vector, index, vector_meta->elem_size),
        elem,
        vector_meta->elem_size
    );
    vector_meta->size++;
    return vector;
}

void *internal_vec_push_back(void *vector, void *elem) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    vector = resize(
        &vector_meta,
        find_new_capacity(vector_meta->capacity, vector_meta->size + 1)
    );
    ASSERT(vector != NULL, "Out of memory");

    // Insert the element.
    memcpy(
        VEC_GET(vector, vector_meta->size, vector_meta->elem_size),
        elem,
        vector_meta->elem_size
    );
    vector_meta->size++;
    return vector;
}

void *internal_vec_extend(void *vector, void *array, size_t size) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    vector = resize(
        &vector_meta,
        find_new_capacity(vector_meta->capacity, vector_meta->size + size)
    );
    ASSERT(vector != NULL, "Out of memory");

    // Copy the array elements to the vector.
    memcpy(
        VEC_GET(vector, vector_meta->size, vector_meta->elem_size),
        array,
        size * vector_meta->elem_size
    );
    vector_meta->size += size;
    return vector;
}

void vec_erase(void *vector, size_t index, void *elem) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    ASSERT(
        index < vector_meta->size,
        "Index (is %zu) should be < vector_size (is %zu)",
        index,
        vector_meta->size
    );

    // Copy the erased element to elem.
    if (elem != NULL) {
        memcpy(
            elem,
            VEC_GET(vector, index, vector_meta->elem_size),
            vector_meta->elem_size
        );
    }

    // Move the elements after index to the left by 1 position.
    vector_meta->size--;
    memmove(
        VEC_GET(vector, index, vector_meta->elem_size),
        VEC_GET(vector, index + 1, vector_meta->elem_size),
        (vector_meta->size - index) * vector_meta->elem_size
    );
}

void vec_swap_erase(void *vector, size_t index, void *elem) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    ASSERT(
        index < vector_meta->size,
        "Index (is %zu) should be < vector_size (is %zu)",
        index,
        vector_meta->size
    );

    // Copy the erased element to elem.
    if (elem != NULL) {
        memcpy(
            elem,
            VEC_GET(vector, index, vector_meta->elem_size),
            vector_meta->elem_size
        );
    }

    // If we are not deleting the last element then swap with last element.
    vector_meta->size--;
    if (index != vector_meta->size) {
        memmove(
            VEC_GET(vector, index, vector_meta->elem_size),
            VEC_GET(vector, vector_meta->size, vector_meta->elem_size),
            vector_meta->elem_size
        );
    }
}

void vec_pop_back(void *vector, void *elem) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    ASSERT(
        vector_meta->size > 0,
        "vector_size (is %zu) should be > 0",
        vector_meta->size
    );

    // Copy the erased element to elem.
    vector_meta->size--;
    if (elem != NULL) {
        memcpy(
            elem,
            VEC_GET(vector, vector_meta->size, vector_meta->elem_size),
            vector_meta->elem_size
        );
    }
}

void vec_clear(void *vector) {
    VEC_META_PTR(vector)->size = 0;
}

void vec_free(void *vector) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    allocator_deallocate(vector_meta->alloc, vector_meta);
}

void *vec_reserve(void *vector, size_t new_capacity) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    if (new_capacity > vector_meta->capacity) {
        vector = resize(&vector_meta, new_capacity);
        ASSERT(vector != NULL, "Out of memory");
    }
    return vector;
}

void *vec_shrink(void *vector) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    vector = resize(&vector_meta, vector_meta->size);
    ASSERT(vector != NULL, "Out of memory");
    return vector;
}

size_t internal_vec_slice(void *vector, void *buffer, VecSliceArgs args) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    ASSERT(
        args.start <= args.end,
        "start (is %zu) should be <= end (is %zu)",
        args.start,
        args.end
    );
    ASSERT(
        args.end <= vector_meta->size,
        "end (is %zu) should be <= vector_size (is %zu)",
        args.end,
        vector_meta->size
    );
    ASSERT(buffer != NULL, "buffer must be a non NULL pointer");

    // Copy elements from vec to the buffer.
    memcpy(
        buffer,
        VEC_GET(vector, args.start, vector_meta->elem_size),
        (args.end - args.start) * vector_meta->elem_size
    );
    return args.end - args.start;
}

void vec_reverse(void *vector) {
    VectorMeta *vector_meta = VEC_META_PTR(vector);
    for (size_t i = 0, j = vector_meta->size - 1; i < j; i++, j--) {
        swap(
            VEC_GET(vector, i, vector_meta->elem_size),
            VEC_GET(vector, j, vector_meta->elem_size),
            vector_meta->elem_size
        );
    }
}

Iterator vec_iter(void *vector) {
    Iterator iterator = iter_default(VEC_META_PTR(vector), vector, vit_next);
    iterator.advance = vit_advance;
    iterator.size = vit_size;
    return iterator;
}

// Resize the vector's capacity to new_capacity updating vector_meta_ref and
// returning the updated vector.
static void *resize(VectorMeta **vector_meta_ref, size_t new_capacity) {
    if (new_capacity != (*vector_meta_ref)->capacity) {
        (*vector_meta_ref)->capacity = new_capacity;
        VectorMeta *vector_meta = allocator_reallocate(
            (*vector_meta_ref)->alloc,
            *vector_meta_ref,
            sizeof(VectorMeta) + ((*vector_meta_ref)->elem_size * new_capacity)
        );

        if (vector_meta == NULL) {
            return NULL;
        }
        *vector_meta_ref = vector_meta;
    }
    return VEC_PTR(*vector_meta_ref);
}

// Find the capacity that is a power of 2 which is greater than or equal to required_capacity.
static size_t find_new_capacity(size_t current_capacity, size_t required_capacity) {
    current_capacity = (current_capacity == 0) ? 1 : current_capacity;
    while (current_capacity < required_capacity) {
        current_capacity *= 2;
    }
    return current_capacity;
}

// Swap 2 pointers of given size.
static void swap(void *ptr1, void *ptr2, size_t size) {
    char temp[size];
    memcpy(temp, ptr1, size);
    memcpy(ptr1, ptr2, size);
    memcpy(ptr2, temp, size);
}

// Move the iterator by 1 element.
static Option vit_next(Iterator *iterator) {
    VectorMeta *vector_meta = iterator->container;
    void *vector = VEC_PTR(vector_meta);
    void *current = iterator->current;
    if (current >= VEC_GET(vector, vector_meta->size, vector_meta->elem_size)) {
        return option_none();
    } else {
        iterator->current = VEC_GET(current, 1, vector_meta->elem_size);
        return option_some(current);
    }
}

// Move the iterator by n elements.
static Option vit_advance(Iterator *iterator, size_t n) {
    VectorMeta *vector_meta = iterator->container;
    void *vector = VEC_PTR(vector_meta);
    void *current = iterator->current;
    if (current >= VEC_GET(vector, vector_meta->size, vector_meta->elem_size)) {
        return option_none();
    } else {
        iterator->current = VEC_GET(current, n, vector_meta->elem_size);
        return option_some(current);
    }
}

// Get the number of elements in the iterator.
static size_t vit_size(Iterator *iterator) {
    VectorMeta *vector_meta = iterator->container;
    void *vector = VEC_PTR(vector_meta);
    void *vector_end = VEC_GET(vector, vector_meta->size, vector_meta->elem_size);
    size_t size = ((size_t) vector_end - (size_t) iterator->current) / vector_meta->elem_size;
    iterator->current = vector_end;
    return size;
}
