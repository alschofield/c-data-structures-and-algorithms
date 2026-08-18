#include "linear_search.h"

#include <assert.h>
#include <stddef.h>

static int compare_ints(const void *left, const void *right) {
    const int *left_value = left;
    const int *right_value = right;

    if (*left_value < *right_value) {
        return -1;
    }
    if (*left_value > *right_value) {
        return 1;
    }
    return 0;
}

static void test_finds_first_match_in_unsorted_input(void) {
    int values[] = { 4, 2, 9, 2, 7 };
    void *items[5];
    int key = 2;
    size_t out_index = 99U;

    for (size_t index = 0U; index < 5U; ++index) {
        items[index] = &values[index];
    }

    assert(linear_search(items, 5U, &key, compare_ints, &out_index));
    assert(out_index == 1U);

    key = 7;
    assert(linear_search(items, 5U, &key, compare_ints, &out_index));
    assert(out_index == 4U);

    key = 4;
    assert(linear_search(items, 5U, &key, compare_ints, &out_index));
    assert(out_index == 0U);
}

static void test_missing_target_reported_distinctly(void) {
    int values[] = { 4, 2, 9 };
    void *items[3];
    int key = 5;
    size_t out_index = 99U;

    for (size_t index = 0U; index < 3U; ++index) {
        items[index] = &values[index];
    }

    assert(!linear_search(items, 3U, &key, compare_ints, &out_index));
    assert(out_index == 99U);
}

static void test_invalid_inputs(void) {
    int values[] = { 4 };
    void *items[] = { &values[0] };
    int key = 4;
    size_t out_index = 99U;

    assert(!linear_search(items, 0U, &key, compare_ints, &out_index));
    assert(!linear_search(NULL, 1U, &key, compare_ints, &out_index));
    assert(!linear_search(items, 1U, &key, compare_ints, NULL));
    assert(out_index == 99U);
}

static void test_large_input(void) {
    enum { ITEM_COUNT = 1024 };
    static int values[ITEM_COUNT];
    static void *items[ITEM_COUNT];
    int key = 0;
    size_t out_index = 0U;

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        values[index] = (int)index;
        items[index] = &values[index];
    }

    key = ITEM_COUNT - 1;
    assert(linear_search(items, ITEM_COUNT, &key, compare_ints, &out_index));
    assert(out_index == (size_t)(ITEM_COUNT - 1));

    key = ITEM_COUNT;
    assert(!linear_search(items, ITEM_COUNT, &key, compare_ints, &out_index));
}

int main(void) {
    test_finds_first_match_in_unsorted_input();
    test_missing_target_reported_distinctly();
    test_invalid_inputs();
    test_large_input();
    return 0;
}
