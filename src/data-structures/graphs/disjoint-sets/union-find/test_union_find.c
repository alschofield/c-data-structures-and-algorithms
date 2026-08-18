#include "union_find.h"

#include <assert.h>
#include <stddef.h>

static void test_singletons(void) {
    UnionFind *set = union_find_create(4U);
    size_t representative = 99U;
    bool connected = true;

    assert(set != NULL);
    assert(union_find_set_count(set) == 4U);
    for (size_t element = 0U; element < 4U; element++) {
        assert(union_find_find(set, element, &representative));
        assert(representative == element);
    }
    assert(union_find_connected(set, 0U, 1U, &connected));
    assert(!connected);
    union_find_destroy(set);
}

static void test_union_merges_and_counts(void) {
    UnionFind *set = union_find_create(6U);
    bool merged = false;
    bool connected = false;
    size_t rep_a = 0U;
    size_t rep_b = 0U;

    assert(set != NULL);
    assert(union_find_union(set, 0U, 1U, &merged));
    assert(merged);
    assert(union_find_set_count(set) == 5U);
    assert(union_find_union(set, 2U, 3U, &merged));
    assert(merged);
    assert(union_find_union(set, 0U, 3U, &merged));
    assert(merged);
    assert(union_find_set_count(set) == 3U);

    assert(union_find_connected(set, 1U, 2U, &connected));
    assert(connected);
    assert(union_find_connected(set, 1U, 4U, &connected));
    assert(!connected);
    assert(union_find_find(set, 1U, &rep_a));
    assert(union_find_find(set, 2U, &rep_b));
    assert(rep_a == rep_b);
    union_find_destroy(set);
}

static void test_redundant_union_is_noop(void) {
    UnionFind *set = union_find_create(3U);
    bool merged = true;

    assert(set != NULL);
    assert(union_find_union(set, 0U, 1U, &merged));
    assert(merged);
    assert(union_find_union(set, 1U, 0U, &merged));
    assert(!merged);
    assert(union_find_set_count(set) == 2U);
    union_find_destroy(set);
}

static void test_out_of_range_elements(void) {
    UnionFind *set = union_find_create(2U);
    size_t representative = 99U;
    bool merged = true;
    bool connected = true;

    assert(set != NULL);
    assert(!union_find_find(set, 2U, &representative));
    assert(representative == 99U);
    assert(!union_find_union(set, 0U, 2U, &merged));
    assert(!union_find_connected(set, 2U, 0U, &connected));
    assert(union_find_set_count(set) == 2U);
    union_find_destroy(set);
}

static void test_null_container(void) {
    size_t representative = 99U;
    bool merged = true;
    bool connected = true;

    assert(!union_find_find(NULL, 0U, &representative));
    assert(!union_find_union(NULL, 0U, 1U, &merged));
    assert(!union_find_connected(NULL, 0U, 1U, &connected));
    assert(union_find_set_count(NULL) == 0U);
    union_find_destroy(NULL);
}

int main(void) {
    test_singletons();
    test_union_merges_and_counts();
    test_redundant_union_is_noop();
    test_out_of_range_elements();
    test_null_container();
    return 0;
}
