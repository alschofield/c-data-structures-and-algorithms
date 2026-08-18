#include "counting_sort.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

static bool values_sorted(const uint32_t *items, size_t count) {
    for (size_t index = 1U; index < count; index++) {
        if (items[index - 1U] > items[index]) {
            return false;
        }
    }
    return true;
}

static void test_invalid_and_trivial_inputs(void) {
    uint32_t single[] = { 3U };

    assert(!counting_sort(NULL, 1U, 10U));
    assert(counting_sort(NULL, 0U, 10U));
    assert(counting_sort(single, 1U, 10U));
    assert(single[0] == 3U);
}

static void test_rejects_out_of_range_keys(void) {
    uint32_t values[] = { 1U, 10U, 2U };

    assert(!counting_sort(values, 3U, 10U));
}

static void test_sorts_keys_within_range(void) {
    uint32_t values[] = { 7U, 3U, 9U, 3U, 0U, 8U, 2U, 7U, 5U, 0U };

    assert(counting_sort(values, 10U, 10U));
    assert(values_sorted(values, 10U));
    assert(values[0] == 0U);
    assert(values[1] == 0U);
    assert(values[9] == 9U);
}

static void test_large_shuffled_input(void) {
    enum { ITEM_COUNT = 1024 };
    static uint32_t values[ITEM_COUNT];
    unsigned int seed = 12345U;

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        seed = seed * 1103515245U + 12345U;
        values[index] = (uint32_t)(seed % 256U);
    }

    assert(counting_sort(values, ITEM_COUNT, 256U));
    assert(values_sorted(values, ITEM_COUNT));
}

int main(void) {
    test_invalid_and_trivial_inputs();
    test_rejects_out_of_range_keys();
    test_sorts_keys_within_range();
    test_large_shuffled_input();
    return 0;
}
