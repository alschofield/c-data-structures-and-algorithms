// Declares the public binary-search API.
#include "binary_search.h"

// Searches one sorted half-open index window [low, high).
static bool search(void *const *items, size_t low, size_t high, const void *key, BinarySearchCompareFn compare, size_t *out_index) {
    // Rejects a missing comparison callback.
    if (compare == NULL) {
        return false;
    }

    // Rejects a missing search key.
    if (key == NULL) {
        return false;
    }

    // Reports a clean miss once the candidate window is empty.
    if (low >= high) {
        return false;
    }

    // Calculates an overflow-safe midpoint inside the current search window.
    size_t mid = low + (high - low) / 2;

    // Discards the lower half, including an already-tested too-small midpoint.
    if (compare(key, items[mid]) > 0) {
        return search(items, mid + 1, high, key, compare, out_index);
    // Discards the upper half, excluding an already-tested too-large midpoint.
    } else if (compare(key, items[mid]) < 0) {
        return search(items, low, mid, key, compare, out_index);
    } else {
        // Returns one matching sorted-array index.
        *out_index = mid;
        return true;
    }
}

// Finds one matching key in caller-provided ascending sorted items.
bool binary_search(void *const *items, size_t count, const void *key, BinarySearchCompareFn compare, size_t *out_index) {
    // Treats empty input as a clean not-found result.
    if (count == 0U) {
        return false;
    }

    // Rejects a missing nonempty input array.
    if (items == NULL) {
        return false;
    }

    // Rejects a missing search key.
    if (key == NULL) {
        return false;
    }

    // Rejects a missing ordering callback.
    if (compare == NULL) {
        return false;
    }

    // Rejects a missing caller-owned output location.
    if (out_index == NULL) {
        return false;
    }

    // Searches the full original array without modifying caller-owned items.
    return search(items, 0U, count, key, compare, out_index);
}
