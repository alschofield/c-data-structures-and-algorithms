#include "binary_search.h"

static bool search(void *const *items, size_t low, size_t high, const void *key, BinarySearchCompareFn compare, size_t *out_index) {
    if (compare == NULL) {
        return false;
    }

    if (key == NULL) {
        return false;
    }

    if (low >= high) {
        return false;
    }
    
    size_t mid = low + (high - low) / 2;

    if (compare(key, items[mid]) > 0) {
        return search(items, mid + 1, high, key, compare, out_index);
    } else if (compare(key, items[mid]) < 0) {
        return search(items, low, mid, key, compare, out_index);
    } else {
        *out_index = mid;
        return true;
    }

    return false;
}

bool binary_search(void *const *items, size_t count, const void *key, BinarySearchCompareFn compare, size_t *out_index) {
    if (count == 0U) {
        return false;
    }
    
    if (items == NULL) {
        return false;
    }

    if (key == NULL) {
        return false;
    }

    if (compare == NULL) {
        return false;
    }

    if (out_index == NULL) {
        return false;
    } 

    return search(items, 0U, count, key, compare, out_index);
}
