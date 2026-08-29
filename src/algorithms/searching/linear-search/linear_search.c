// Declares the public linear-search API.
#include "linear_search.h"

// Finds the first item equal to a caller-provided key by sequential scan.
bool linear_search(void *const *items, size_t count, const void *key, LinearSearchCompareFn compare, size_t *out_index) {
    // Rejects a missing input array.
    if (items == NULL) {
        return false;
    }

    // Rejects a missing search key.
    if (key == NULL) {
        return false;
    }

    // Rejects a missing equality/ordering callback.
    if (compare == NULL) {
        return false;
    }

    // Rejects a missing caller-owned output location.
    if (out_index == NULL) {
        return false;
    }

    // Treats empty input as a clean not-found result.
    if (count == 0U) {
        return false;
    }

    // Examines every input item from lowest index to highest index.
    size_t n = 0U;
    while(n < count) {
        // Reports the first equal item, preserving duplicate ordering semantics.
        if (compare(key, items[n]) == 0U) {
            *out_index = n;
            return true;
        }

        // Advances only after the current item has been tested.
        n++;
    }

    // Reports a missing key without changing the caller's output index.
    return false;
}
