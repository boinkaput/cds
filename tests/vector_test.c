#include <assert.h>

#include "../vector.h"

void test_vector_basic() {
    int (*vec) = vec_new(int);
    assert(vec_size(vec) == 0);
    assert(vec_capacity(vec) == 0);
    assert(vec_is_empty(vec));

    vec_push_back(vec, 10);
    vec_push_back(vec, 20);
    vec_push_back(vec, 30);
    vec_push_back(vec, 100);
    vec_insert(vec, 70, 3);
    assert(vec_size(vec) == 5);
    assert(vec_capacity(vec) >= 5);

    assert(vec[0] == 10);
    assert(vec[1] == 20);
    assert(vec[2] == 30);
    assert(vec[3] == 70);
    assert(vec[4] == 100);

    int last_element;
    vec_pop_back(vec, &last_element);
    assert(last_element == 100);
    assert(vec_size(vec) == 4);

    int erased_element;
    vec_erase(vec, 1, &erased_element);
    assert(erased_element == 20);
    assert(vec_size(vec) == 3);
    assert(vec[0] == 10);
    assert(vec[1] == 30);
    assert(vec[2] == 70);

    vec_clear(vec);
    assert(vec_size(vec) == 0);
    assert(vec_capacity(vec) >= 3);
    assert(vec_is_empty(vec));

    vec_free(vec);
}

void test_vector_with_capacity() {
    Vec(double) vec = vec_new(double, .cap = 10);
    assert(vec_size(vec) == 0);
    assert(vec_capacity(vec) >= 10);

    for (int i = 0; i < 8; i++) {
        vec_push_back(vec, i * 1.5);
    }
    assert(vec_size(vec) == 8);

    for (int i = 0; i < 15; i++) {
        vec_push_back(vec, i * 2.0);
    }
    assert(vec_size(vec) == 23);

    assert(vec[0] == 0.0);
    assert(vec[7] == 10.5);
    assert(vec[8] == 0.0);
    assert(vec[22] == 28.0);

    vec_free(vec);
}

void test_vector_from_array() {
    Vec(int) vec1 = vec_from_array(((int[]) {1, 2, 3, 4, 5}), 5);
    assert(vec_size(vec1) == 5);
    assert(vec_capacity(vec1) >= 5);
    assert(vec1[0] == 1);
    assert(vec1[2] == 3);
    assert(vec1[4] == 5);

    Vec(int) vec2 = vec_from_array(vec1, vec_size(vec1));
    assert(vec_size(vec2) == 5);
    assert(vec_capacity(vec2) >= 5);
    assert(vec2[1] == 2);
    assert(vec2[3] == 4);

    vec_free(vec1);
    vec_free(vec2);
}

void test_vector_insert() {
    Vec(int) vec = vec_new(int);

    for (int i = 0; i < 5; i++) {
        vec_push_back(vec, i);
    }

    vec_insert(vec, 99, 2);
    assert(vec_size(vec) == 6);
    assert(vec[2] == 99);

    vec_free(vec);
}

void test_vector_extend() {
    int arr1[] = {1, 2, 3};
    int arr2[] = {4, 5, 6, 7};

    Vec(int) vec = vec_from_array(arr1, 3);
    assert(vec_size(vec) == 3);

    vec_extend(vec, arr2, 4);
    assert(vec_size(vec) == 7);
    assert(vec[0] == 1);
    assert(vec[6] == 7);

    vec_free(vec);
}

void test_vector_reserve_and_shrink() {
    Vec(int) vec = vec_new(int);
    vec = vec_reserve(vec, 10);
    assert(vec_size(vec) == 0);
    assert(vec_capacity(vec) >= 10);

    for (int i = 0; i < 5; i++) {
        vec_push_back(vec, i * 5);
    }
    assert(vec_size(vec) == 5);

    vec = vec_shrink(vec);
    assert(vec_size(vec) == 5);
    assert(vec_capacity(vec) == 5);

    for (int i = 0; i < 10; i++) {
        vec_push_back(vec, i * 10);
    }
    assert(vec_size(vec) == 15);
    assert(vec_capacity(vec) >= 15);

    vec = vec_reserve(vec, 20);
    assert(vec_size(vec) == 15);
    assert(vec_capacity(vec) >= 20);

    vec_free(vec);
}

void test_vector_reverse() {
    Vec(int) vec = vec_new(int);

    for (int i = 0; i < 10; i++) {
        vec_push_back(vec, i * 2);
    }

    vec_reverse(vec);
    assert(vec_size(vec) == 10);
    assert(vec[0] == 18);
    assert(vec[1] == 16);
    assert(vec[2] == 14);
    assert(vec[3] == 12);
    assert(vec[4] == 10);
    assert(vec[5] == 8);
    assert(vec[6] == 6);
    assert(vec[7] == 4);
    assert(vec[8] == 2);
    assert(vec[9] == 0);

    vec_reverse(vec);
    assert(vec[0] == 0);
    assert(vec[1] == 2);
    assert(vec[2] == 4);
    assert(vec[3] == 6);
    assert(vec[4] == 8);
    assert(vec[5] == 10);
    assert(vec[6] == 12);
    assert(vec[7] == 14);
    assert(vec[8] == 16);
    assert(vec[9] == 18);

    vec_free(vec);
}

void test_vector_pop_back() {
    Vec(int) vec = vec_new(int);

    for (int i = 0; i < 5; i++) {
        vec_push_back(vec, i);
    }

    int popped;
    vec_pop_back(vec, &popped);
    assert(popped == 4);
    assert(vec_size(vec) == 4);

    vec_pop_back(vec, &popped);
    assert(popped == 3);
    assert(vec_size(vec) == 3);

    vec_pop_back(vec, NULL);
    assert(vec_size(vec) == 2);

    vec_free(vec);
}

void test_vector_clear() {
    Vec(int) vec = vec_new(int);

    for (int i = 0; i < 5; i++) {
        vec_push_back(vec, i);
    }

    vec_clear(vec);
    assert(vec_is_empty(vec));

    vec_free(vec);
}

void test_vector_erase() {
    Vec(int) vec = vec_new(int);

    for (int i = 0; i < 5; i++) {
        vec_push_back(vec, i);
    }

    int erased;
    vec_erase(vec, 2, &erased);
    assert(erased == 2);
    assert(vec_size(vec) == 4);
    assert(vec[2] == 3);

    vec_erase(vec, 0, &erased);
    assert(erased == 0);
    assert(vec_size(vec) == 3);
    assert(vec[0] == 1);

    vec_erase(vec, 2, NULL);
    assert(vec_size(vec) == 2);
    assert(vec[1] == 3);

    vec_free(vec);
}

void test_vector_swap_erase() {
    Vec(int) vec = vec_new(int);

    for (int i = 0; i < 5; i++) {
        vec_push_back(vec, i);
    }

    int erased;
    vec_swap_erase(vec, 2, &erased);
    assert(erased == 2);
    assert(vec_size(vec) == 4);
    assert(vec[2] == 4);

    vec_swap_erase(vec, 0, &erased);
    assert(erased == 0);
    assert(vec_size(vec) == 3);
    assert(vec[0] == 3);

    vec_swap_erase(vec, 2, NULL);
    assert(vec_size(vec) == 2);
    assert(vec[1] == 1);

    vec_free(vec);
}

void test_vector_slice() {
    Vec(size_t) vec = vec_new(size_t);

    for (size_t i = 0; i < 10; i++) {
        vec_push_back(vec, i);
    }

    size_t slice1[5];
    size_t size = vec_slice(vec, slice1, .start = 2, .end = 7);
    assert(size == 5);
    for (size_t i = 0; i < size; i++) {
        assert(slice1[i] == i + 2);
    }

    size_t slice2[8];
    size = vec_slice(vec, slice2, .start = 2);
    assert(size == 8);
    for (size_t i = 0; i < size; i++) {
        assert(slice2[i] == i + 2);
    }

    size_t slice3[7];
    size = vec_slice(vec, slice3, .end = 7);
    assert(size == 7);
    for (size_t i = 0; i < size; i++) {
        assert(slice3[i] == i);
    }

    size_t slice4[10];
    size = vec_slice(vec, slice4);
    assert(size == 10);
    for (size_t i = 0; i < size; i++) {
        assert(slice4[i] == i);
    }

    vec_free(vec);
}

struct Person {
    char name[50];
    int age;
};

typedef struct Person People[3];

void test_vector_person_struct() {
    People people_data = {
        {"Alice", 25},
        {"Bob", 30},
        {"Charlie", 28}
    };

    Vec(struct Person) people = vec_new(struct Person);

    for (size_t i = 0; i < 3; i++) {
        vec_push_back(people, people_data[i]);
    }

    assert(vec_size(people) == 3);
    for (size_t i = 0; i < vec_size(people); i++) {
        assert(people[i].age == people_data[i].age);
        assert(strcmp(people[i].name, people_data[i].name) == 0);
    }

    vec_extend(people, people_data, 3);
    assert(vec_size(people) == 6);
    for (size_t i = 3; i < vec_size(people); i++) {
        assert(people[i].age == people_data[i - 3].age);
        assert(strcmp(people[i].name, people_data[i - 3].name) == 0);
    }

    vec_free(people);
}

void test_vector_person_struct_array() {
    People people_data[] = {
        {{"Alice", 25}, {"Bob", 30}, {"Charlie", 28}},
        {{"David", 22}, {"Eve", 29}, {"Frank", 35}},
        {{"Grace", 27}, {"Henry", 31}, {"Ivy", 24}}
    };

    Vec(People) people_vectors = vec_from_array(people_data, 3);

    assert(vec_size(people_vectors) == 3);
    for (size_t i = 0; i < vec_size(people_vectors); i++) {
        for (size_t j = 0; j < 3; j++) {
            assert(people_vectors[i][j].age == people_data[i][j].age);
            assert(strcmp(people_vectors[i][j].name, people_data[i][j].name) == 0);
        }
    }

    vec_extend(people_vectors, people_data, 3);
    assert(vec_size(people_vectors) == 6);
    for (size_t i = 3; i < vec_size(people_vectors); i++) {
        for (size_t j = 0; j < 3; j++) {
            assert(people_vectors[i][j].age == people_data[i - 3][j].age);
            assert(strcmp(people_vectors[i][j].name, people_data[i - 3][j].name) == 0);
        }
    }

    vec_free(people_vectors);
}

void test_2d_vector_person_struct() {
    People people_data[] = {
        {{"Alice", 25}, {"Bob", 30}, {"Charlie", 28}},
        {{"David", 22}, {"Eve", 29}, {"Frank", 35}},
        {{"Grace", 27}, {"Henry", 31}, {"Ivy", 24}}
    };

    struct Person *people_vector_data[3] = {
        vec_from_array(people_data[0], 3),
        vec_from_array(people_data[1], 3),
        vec_from_array(people_data[2], 3)
    };

    Vec(Vec(struct Person)) people_vectors = vec_from_array(people_vector_data, 3);
    assert(vec_size(people_vectors) == 3);
    for (size_t i = 0; i < vec_size(people_vectors); i++) {
        assert(vec_size(people_vectors[i]) == 3);
        for (size_t j = 0; j < 3; j++) {
            assert(people_vectors[i][j].age == people_data[i][j].age);
            assert(strcmp(people_vectors[i][j].name, people_data[i][j].name) == 0);
        }
    }

    vec_extend(people_vectors, people_vector_data, 3);
    assert(vec_size(people_vectors) == 6);
    for (size_t i = 3; i < vec_size(people_vectors); i++) {
        assert(vec_size(people_vectors[i]) == 3);
        for (size_t j = 0; j < 3; j++) {
            assert(people_vectors[i][j].age == people_data[i - 3][j].age);
            assert(strcmp(people_vectors[i][j].name, people_data[i - 3][j].name) == 0);
        }
    }

    for (int i = 0; i < 3; i++) {
        vec_free(people_vector_data[i]);
    }

    vec_free(people_vectors);
}

int int_compare(int *val1, int *val2) {
    return (*val1 - *val2);
}

void test_vector_sort() {
    Vec(int) vec = vec_new(int);
    vec_sort(vec, (compare_fn) int_compare);
    assert(vec_is_empty(vec));
    vec_push_back(vec, 42);
    vec_sort(vec, (compare_fn) int_compare);
    assert(vec_size(vec) == 1);
    assert(vec[0] == 42);
    vec_free(vec);

    Vec(int) vec1 = vec_new(int);
    for (int i = 1; i <= 5; i++) {
        vec_push_back(vec1, i);
    }
    vec_sort(vec1, (compare_fn) int_compare);
    assert(vec_size(vec1) == 5);
    for (int i = 0; i < 5; i++) {
        assert(vec1[i] == i + 1);
    }
    vec_free(vec1);

    Vec(int) vec2 = vec_from_array(((int[]) {5, 2, 9, 1, 7}), 5);
    vec_sort(vec2, (compare_fn) int_compare);
    assert(vec_size(vec2) == 5);
    assert(vec2[0] == 1);
    assert(vec2[1] == 2);
    assert(vec2[2] == 5);
    assert(vec2[3] == 7);
    assert(vec2[4] == 9);
    vec_free(vec2);

    Vec(int) vec3 = vec_new(int);
    for (int i = 5; i > 0; i--) {
        vec_push_back(vec3, i);
    }
    vec_sort(vec3, (compare_fn) int_compare);
    assert(vec_size(vec3) == 5);
    for (int i = 0; i < 5; i++) {
        assert(vec3[i] == i + 1);
    }
    vec_free(vec3);

    Vec(int) vec4 = vec_new(int);
    for (int i = 5; i > 0; i--) {
        vec_push_back(vec4, i);
        vec_push_back(vec4, i);
    }
    vec_sort(vec4, (compare_fn) int_compare);
    assert(vec_size(vec4) == 10);
    for (int i = 0; i < 10; i++) {
        assert(vec4[i] == (i / 2) + 1);
    }
    vec_free(vec4);

    Vec(int) vec5 = vec_new(int);
    vec_push_back(vec5, -5);
    vec_push_back(vec5, -3);
    vec_push_back(vec5, -9);
    vec_push_back(vec5, -1);
    vec_push_back(vec5, -7);
    vec_sort(vec5, (compare_fn) int_compare);
    assert(vec_size(vec5) == 5);
    for (int i = 0; i < 5; i++) {
        assert(vec5[i] == -9 + (i * 2));
    }
    vec_free(vec5);

    Vec(int) vec6 = vec_new(int);
    vec_push_back(vec6, -3);
    vec_push_back(vec6, 7);
    vec_push_back(vec6, 0);
    vec_push_back(vec6, -1);
    vec_push_back(vec6, 5);
    vec_sort(vec6, (compare_fn) int_compare);
    assert(vec_size(vec6) == 5);
    assert(vec6[0] == -3);
    assert(vec6[1] == -1);
    assert(vec6[2] == 0);
    assert(vec6[3] == 5);
    assert(vec6[4] == 7);
    vec_free(vec6);
}

int main() {
    test_vector_basic();
    test_vector_with_capacity();
    test_vector_from_array();
    test_vector_insert();
    test_vector_extend();
    test_vector_reserve_and_shrink();
    test_vector_reverse();
    test_vector_pop_back();
    test_vector_clear();
    test_vector_erase();
    test_vector_swap_erase();
    test_vector_slice();
    test_vector_person_struct();
    test_vector_person_struct_array();
    test_2d_vector_person_struct();
    test_vector_sort();
    return 0;
}
