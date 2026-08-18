#include "radix_sort.h"

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

    assert(!radix_sort(NULL, 1U));
    assert(radix_sort(NULL, 0U));
    assert(radix_sort(single, 1U));
    assert(single[0] == 3U);
}

static void test_sorts_full_value_range(void) {
    uint32_t values[] = {
        4294967295U, 0U, 65536U, 255U, 256U, 4294967294U, 1U, 16777216U
    };

    assert(radix_sort(values, 8U));
    assert(values_sorted(values, 8U));
    assert(values[0] == 0U);
    assert(values[7] == 4294967295U);
}

static void test_sorts_duplicates(void) {
    uint32_t values[] = { 9U, 4U, 9U, 4U, 9U, 4U };

    assert(radix_sort(values, 6U));
    assert(values_sorted(values, 6U));
}

static void test_large_shuffled_input(void) {
    enum { ITEM_COUNT = 1024 };
    static uint32_t values[ITEM_COUNT];
    unsigned int seed = 12345U;

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        seed = seed * 1103515245U + 12345U;
        values[index] = (uint32_t)seed * 2654435761U;
    }

    assert(radix_sort(values, ITEM_COUNT));
    assert(values_sorted(values, ITEM_COUNT));
}

int main(void) {
    test_invalid_and_trivial_inputs();
    test_sorts_full_value_range();
    test_sorts_duplicates();
    test_large_shuffled_input();
    return 0;
}
