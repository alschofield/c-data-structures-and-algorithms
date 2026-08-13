#include "search.h"

bool linear_search(
    void *const *items,
    size_t count,
    const void *key,
    SearchCompareFn compare,
    size_t *out_index
) {
    (void)items;
    (void)count;
    (void)key;
    (void)compare;
    (void)out_index;

    // TODO: Validate inputs, then compare every item from index zero onward.
    return false;
}

bool binary_search(
    void *const *items,
    size_t count,
    const void *key,
    SearchCompareFn compare,
    size_t *out_index
) {
    (void)items;
    (void)count;
    (void)key;
    (void)compare;
    (void)out_index;

    // TODO: Validate inputs, then narrow a sorted inclusive/exclusive range.
    return false;
}
