#include "insertion_sort.h"

#include <assert.h>
#include <stddef.h>

struct Record {
    int key;
    int sequence;
};

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
    for (size_t index = 1U; index < count; ++index) {
        if (compare_ints(items[index - 1U], items[index]) > 0) {
            return false;
        }
    }
    return true;
}

static int compare_record_keys(const void *left, const void *right) {
    const struct Record *left_record = left;
    const struct Record *right_record = right;

    if (left_record->key < right_record->key) {
        return -1;
    }
    if (left_record->key > right_record->key) {
        return 1;
    }
    return 0;
}

static void test_invalid_and_trivial_inputs(void) {
    int value = 5;
    void *single[] = { &value };

    assert(!insertion_sort(NULL, 1U, compare_ints));
    assert(insertion_sort(NULL, 0U, compare_ints));
    assert(insertion_sort(single, 1U, compare_ints));
    assert(single[0] == &value);
}

static void test_sorts_shuffled_values(void) {
    int values[] = { 7, 3, 9, 3, 1, 8, 2, 7, 5, 0 };
    void *items[10];
    size_t found = 0U;

    for (size_t index = 0U; index < 10U; ++index) {
        items[index] = &values[index];
    }

    assert(insertion_sort(items, 10U, compare_ints));
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

    for (size_t index = 0U; index < 6U; ++index) {
        items[index] = &ascending[index];
    }
    assert(insertion_sort(items, 6U, compare_ints));
    assert(ints_sorted(items, 6U));

    for (size_t index = 0U; index < 6U; ++index) {
        items[index] = &descending[index];
    }
    assert(insertion_sort(items, 6U, compare_ints));
    assert(ints_sorted(items, 6U));

    for (size_t index = 0U; index < 4U; ++index) {
        items[index] = &equal[index];
    }
    assert(insertion_sort(items, 4U, compare_ints));
    assert(ints_sorted(items, 4U));
}

static void test_equal_keys_keep_original_order(void) {
    struct Record records[] = {
        { .key = 2, .sequence = 0 }, { .key = 1, .sequence = 1 },
        { .key = 2, .sequence = 2 }, { .key = 1, .sequence = 3 },
        { .key = 2, .sequence = 4 }, { .key = 1, .sequence = 5 }
    };
    void *items[6];

    for (size_t index = 0U; index < 6U; ++index) {
        items[index] = &records[index];
    }

    assert(insertion_sort(items, 6U, compare_record_keys));
    assert(((const struct Record *)items[0])->sequence == 1);
    assert(((const struct Record *)items[1])->sequence == 3);
    assert(((const struct Record *)items[2])->sequence == 5);
    assert(((const struct Record *)items[3])->sequence == 0);
    assert(((const struct Record *)items[4])->sequence == 2);
    assert(((const struct Record *)items[5])->sequence == 4);
}

static void test_large_shuffled_input(void) {
    enum { ITEM_COUNT = 1024 };
    static int values[ITEM_COUNT];
    static void *items[ITEM_COUNT];
    unsigned int seed = 12345U;

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        seed = seed * 1103515245U + 12345U;
        values[index] = (int)(seed % 100000U);
        items[index] = &values[index];
    }

    assert(insertion_sort(items, ITEM_COUNT, compare_ints));
    assert(ints_sorted(items, ITEM_COUNT));
}

int main(void) {
    test_invalid_and_trivial_inputs();
    test_sorts_shuffled_values();
    test_sorted_reverse_and_equal_inputs();
    test_equal_keys_keep_original_order();
    test_large_shuffled_input();
    return 0;
}
