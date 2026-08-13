#include "search.h"

#include <assert.h>

struct Record {
    int id;
};

static int record_compare(const void *left, const void *right) {
    const struct Record *left_record = left;
    const struct Record *right_record = right;

    return (left_record->id > right_record->id) - (left_record->id < right_record->id);
}

static void test_linear_search(void) {
    struct Record first = { .id = 3 };
    struct Record second = { .id = 1 };
    struct Record third = { .id = 2 };
    struct Record key = { .id = 1 };
    void *items[] = { &first, &second, &third };
    size_t index = 99U;

    assert(linear_search(items, 3U, &key, record_compare, &index));
    assert(index == 1U);
    assert(!linear_search(items, 3U, &(struct Record){ .id = 4 }, record_compare, &index));
    assert(index == 1U);
}

static void test_binary_search(void) {
    struct Record first = { .id = 1 };
    struct Record second = { .id = 2 };
    struct Record third = { .id = 3 };
    struct Record key = { .id = 3 };
    void *items[] = { &first, &second, &third };
    size_t index = 99U;

    assert(binary_search(items, 3U, &key, record_compare, &index));
    assert(index == 2U);
    assert(!binary_search(items, 3U, &(struct Record){ .id = 4 }, record_compare, &index));
    assert(index == 2U);
    assert(!binary_search(NULL, 0U, &key, record_compare, &index));
}

int main(void) {
    test_linear_search();
    test_binary_search();
    return 0;
}
