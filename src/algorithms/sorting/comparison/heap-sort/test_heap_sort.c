#include "heap_sort.h"

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

static bool ints_sorted(void *const *items, size_t count) {
    for (size_t index = 1U; index < count; index++) {
        if (compare_ints(items[index - 1U], items[index]) > 0) {
            return false;
        }
    }
    return true;
}

static void test_invalid_and_trivial_inputs(void) {
    int value = 5;
    void *single[] = { &value };

    assert(!heap_sort(NULL, 1U, compare_ints));
    assert(heap_sort(NULL, 0U, compare_ints));
    assert(heap_sort(single, 1U, compare_ints));
    assert(single[0] == &value);
}

static void test_sorts_shuffled_values(void) {
    int values[] = { 7, 3, 9, 3, 1, 8, 2, 7, 5, 0 };
    void *items[10];
    size_t found = 0U;

    for (size_t index = 0U; index < 10U; index++) {
        items[index] = &values[index];
    }

    assert(heap_sort(items, 10U, compare_ints));
    assert(ints_sorted(items, 10U));

    for (size_t original = 0U; original < 10U; original++) {
        for (size_t position = 0U; position < 10U; position++) {
            if (items[position] == &values[original]) {
                found++;
                break;
            }
        }
    }
    assert(found == 10U);
}

static void test_sorted_reverse_and_equal_inputs(void) {
    int ascending[] = { 1, 2, 3, 4, 5, 6 };
    int descending[] = { 6, 5, 4, 3, 2, 1 };
    int equal[] = { 4, 4, 4, 4 };
    void *items[6];

    for (size_t index = 0U; index < 6U; index++) {
        items[index] = &ascending[index];
    }
    assert(heap_sort(items, 6U, compare_ints));
    assert(ints_sorted(items, 6U));

    for (size_t index = 0U; index < 6U; index++) {
        items[index] = &descending[index];
    }
    assert(heap_sort(items, 6U, compare_ints));
    assert(ints_sorted(items, 6U));

    for (size_t index = 0U; index < 4U; index++) {
        items[index] = &equal[index];
    }
    assert(heap_sort(items, 4U, compare_ints));
    assert(ints_sorted(items, 4U));
}

static void test_large_shuffled_input(void) {
    enum { ITEM_COUNT = 1024 };
    static int values[ITEM_COUNT];
    static void *items[ITEM_COUNT];
    unsigned int seed = 12345U;

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        seed = seed * 1103515245U + 12345U;
        values[index] = (int)(seed % 100000U);
        items[index] = &values[index];
    }

    assert(heap_sort(items, ITEM_COUNT, compare_ints));
    assert(ints_sorted(items, ITEM_COUNT));
}

int main(void) {
    test_invalid_and_trivial_inputs();
    test_sorts_shuffled_values();
    test_sorted_reverse_and_equal_inputs();
    test_large_shuffled_input();
    return 0;
}
