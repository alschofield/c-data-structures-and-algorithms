#include "sorting.h"

static bool sort_unimplemented(void **items, size_t count, SortCompareFn compare) {
    (void)items;
    (void)count;
    (void)compare;

    // TODO: Replace this shared scaffold with each algorithm's implementation.
    return false;
}

bool bubble_sort(void **items, size_t count, SortCompareFn compare) {
    return sort_unimplemented(items, count, compare);
}

bool insertion_sort(void **items, size_t count, SortCompareFn compare) {
    return sort_unimplemented(items, count, compare);
}

bool selection_sort(void **items, size_t count, SortCompareFn compare) {
    return sort_unimplemented(items, count, compare);
}

bool merge_sort(void **items, size_t count, SortCompareFn compare) {
    return sort_unimplemented(items, count, compare);
}

bool quick_sort(void **items, size_t count, SortCompareFn compare) {
    return sort_unimplemented(items, count, compare);
}
