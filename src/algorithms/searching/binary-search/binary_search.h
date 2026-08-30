// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Compares a search key and one caller-owned item using qsort-style ordering.
typedef int (*BinarySearchCompareFn)(const void *left, const void *right);

// Returns one matching ascending sorted-item index through out_index.
bool binary_search(void *const *items, size_t count, const void *key, BinarySearchCompareFn compare, size_t *out_index);
