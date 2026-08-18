#include "binary_search.h"

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

static void test_finds_every_element(void) {
    int values[] = { 1, 3, 5, 8, 13, 21 };
    void *items[6];
    size_t out_index = 99U;

    for (size_t index = 0U; index < 6U; ++index) {
        items[index] = &values[index];
    }

    for (size_t index = 0U; index < 6U; ++index) {
        assert(binary_search(items, 6U, &values[index], compare_ints, &out_index));
        assert(out_index == index);
    }
}

static void test_duplicates_return_any_matching_index(void) {
    int values[] = { 1, 3, 3, 3, 8 };
    void *items[5];
    int key = 3;
    size_t out_index = 99U;

    for (size_t index = 0U; index < 5U; ++index) {
        items[index] = &values[index];
    }

    assert(binary_search(items, 5U, &key, compare_ints, &out_index));
    assert(out_index >= 1U);
    assert(out_index <= 3U);
}

static void test_missing_target_reported_distinctly(void) {
    int values[] = { 1, 3, 5, 8 };
    void *items[4];
    int low_key = 0;
    int gap_key = 4;
    int high_key = 9;
    size_t out_index = 99U;

    for (size_t index = 0U; index < 4U; ++index) {
        items[index] = &values[index];
    }

    assert(!binary_search(items, 4U, &low_key, compare_ints, &out_index));
    assert(!binary_search(items, 4U, &gap_key, compare_ints, &out_index));
    assert(!binary_search(items, 4U, &high_key, compare_ints, &out_index));
    assert(out_index == 99U);
}

static void test_invalid_inputs(void) {
    int values[] = { 4 };
    void *items[] = { &values[0] };
    int key = 4;
    size_t out_index = 99U;

    assert(!binary_search(items, 0U, &key, compare_ints, &out_index));
    assert(!binary_search(NULL, 1U, &key, compare_ints, &out_index));
    assert(!binary_search(items, 1U, &key, compare_ints, NULL));
    assert(out_index == 99U);
}

static void test_large_sorted_input(void) {
    enum { ITEM_COUNT = 1024 };
    static int values[ITEM_COUNT];
    static void *items[ITEM_COUNT];
    int absent_key = 1;
    size_t out_index = 0U;

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        values[index] = (int)(index * 2U);
        items[index] = &values[index];
    }

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        assert(binary_search(items, ITEM_COUNT, &values[index], compare_ints, &out_index));
        assert(out_index == index);
    }

    assert(!binary_search(items, ITEM_COUNT, &absent_key, compare_ints, &out_index));
}

int main(void) {
    test_finds_every_element();
    test_duplicates_return_any_matching_index();
    test_missing_target_reported_distinctly();
    test_invalid_inputs();
    test_large_sorted_input();
    return 0;
}
