#include "sorting.h"

#include <assert.h>

struct Record {
    int id;
};

static int record_compare(const void *left, const void *right) {
    const struct Record *left_record = left;
    const struct Record *right_record = right;

    return (left_record->id > right_record->id) - (left_record->id < right_record->id);
}

static void assert_sorted(void **items, size_t count) {
    for (size_t index = 1U; index < count; index++) {
        assert(record_compare(items[index - 1U], items[index]) <= 0);
    }
}

static void test_sort(bool (*sort)(void **items, size_t count, SortCompareFn compare)) {
    struct Record first = { .id = 3 };
    struct Record second = { .id = 1 };
    struct Record third = { .id = 2 };
    void *items[] = { &first, &second, &third };

    assert(sort(items, 3U, record_compare));
    assert_sorted(items, 3U);
    assert(sort(NULL, 0U, record_compare));
    assert(!sort(items, 3U, NULL));
}

int main(void) {
    test_sort(bubble_sort);
    test_sort(insertion_sort);
    test_sort(selection_sort);
    test_sort(merge_sort);
    test_sort(quick_sort);
    return 0;
}
