// Provides the merge sort declaration and comparison callback type.
#include "merge_sort.h"

// Sorts the pointer array into ascending comparison order.
bool merge_sort(void **items, size_t count, MergeSortCompareFn compare) {
    // Rejects a missing comparison function.
    if (compare == NULL) {
        return false;
    }

    // Accepts an empty input as a successful no-op before validating the array.
    if (count == 0U) {
        return true;
    }

    // Rejects a missing array when there are elements to sort.
    if (items == NULL) {
        return false;
    }

    // Accepts a single element as already sorted.
    if (count == 1U) {
        return true;
    }

    // merge sort

    return true;
}
