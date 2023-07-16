#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../option.h"

void test_option_none() {
    Option opt = option_none();
    assert(!opt.is_valid);
    assert(option_unwrap_or(opt, int, -1) == -1);
}

void test_option_some() {
    int i = 1;
    Option opt = option_some(&i);
    assert(opt.is_valid);
    assert(option_unwrap(opt, int) == 1);
}

void test_option_or() {
    int i = 1;
    int j = 3;
    Option opt1 = option_some(&i);
    Option opt2 = option_some(&j);
    Option opt3 = option_none();
    Option opt4 = option_none();
    assert(option_unwrap(option_or(opt1, opt2), int) == 1);
    assert(option_unwrap(option_or(opt1, opt3), int) == 1);
    assert(option_unwrap(option_or(opt3, opt2), int) == 3);
    assert(!option_or(opt3, opt4).is_valid);
}

void test_option_and() {
    int i = 1;
    int j = 3;
    Option opt1 = option_some(&i);
    Option opt2 = option_some(&j);
    Option opt3 = option_none();
    Option opt4 = option_none();
    assert(option_unwrap(option_and(opt1, opt2), int) == 3);
    assert(!option_and(opt1, opt3).is_valid);
    assert(!option_and(opt3, opt2).is_valid);
    assert(!option_and(opt3, opt4).is_valid);
}

Option then(int *k) {
    *k += 5;
    return option_some(k);
}

void test_option_and_then() {
    int i = 1;
    Option opt1 = option_some(&i);
    Option opt2 = option_none();
    assert(option_unwrap(option_and_then(opt1, (map_opt_fn) then), int) == 6);
    assert(!option_and_then(opt2, (map_opt_fn) then).is_valid);
}

void map(int *k) {
    *k += 3;
}

void test_option_map() {
    int i = 1;
    Option opt1 = option_some(&i);
    Option opt2 = option_none();
    option_map(opt1, (map_fn) map);
    assert(option_unwrap(opt1, int) == 4);
    option_map(opt2, (map_fn) map);
    assert(!opt2.is_valid);
}

bool filter1(int *k) {
    return *k == 1;
}

bool filter2(int *k) {
    return *k == 3;
}

void test_option_filter() {
    int i = 1;
    Option opt1 = option_some(&i);
    Option opt2 = option_none();
    assert(option_unwrap(option_filter(opt1, (pred_fn) filter1), int) == 1);
    assert(!option_filter(opt1, (pred_fn) filter2).is_valid);
    assert(!option_filter(opt2, (pred_fn) filter2).is_valid);
}

void test_option_flatten() {
    int i = 1;
    Option opt1 = option_some(&i);
    Option opt2 = option_none();
    Option opt3 = option_some(&opt1);
    Option opt4 = option_some(&opt2);
    assert(option_unwrap(option_flatten(opt3), int) == 1);
    assert(!option_flatten(opt4).is_valid);
    assert(!option_flatten(opt2).is_valid);
}

void test_option_alloc() {
    int *i = malloc(sizeof(int));
    *i = 5;
    Option opt = option_some(i);
    assert(option_unwrap(opt, int) == 5);
    option_map(opt, (map_fn) map);
    assert(option_unwrap(option_and_then(opt, (map_opt_fn) then), int) == 13);
    assert(!option_filter(opt, (pred_fn) filter1).is_valid);
    free(opt.value);
}

struct complex {
    int i;
    char c;
    float f;
    double d;
};

bool complex_equal(struct complex comp1, struct complex comp2) {
    return comp1.i == comp2.i
        && comp1.c == comp2.c
        && comp1.f == comp2.f
        && comp1.d == comp2.d;
}

void map_complex(struct complex *comp) {
    comp->c = 'y';
    comp->i = 101;
}

bool filter_complex1(struct complex *comp) {
    return comp->c == 'y' && comp->i == 101;
}

bool filter_complex2(struct complex *comp) {
    return comp->c == 'y' && comp->d == 0;
}

void test_option_complex() {
    struct complex comp = {
        .i = 3,
        .c = 'f',
        .f = -7.1,
        .d = 105.4551
    };
    Option opt = option_some(&comp);
    assert(complex_equal(option_unwrap(opt, struct complex),
                         (struct complex) { .i = 3, .c = 'f', .f = -7.1, .d = 105.4551 }));
    option_map(opt, (map_fn) map_complex);
    assert(complex_equal(option_unwrap(opt, struct complex),
                         (struct complex) { .i = 101, .c = 'y', .f = -7.1, .d = 105.4551 }));
    assert(complex_equal(option_unwrap(option_filter(opt, (pred_fn) filter_complex1), struct complex),
                         (struct complex) { .i = 101, .c = 'y', .f = -7.1, .d = 105.4551 }));
    assert(!option_filter(opt, (pred_fn) filter_complex2).is_valid);
}

int main(int argc, char *argv[]) {
    test_option_none();
    test_option_some();
    test_option_or();
    test_option_and();
    test_option_and_then();
    test_option_map();
    test_option_filter();
    test_option_alloc();
    test_option_complex();
    return 0;
}
