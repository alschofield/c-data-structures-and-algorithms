// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Compares a search key and one caller-owned item using qsort-style ordering.
typedef int (*LinearSearchCompareFn)(const void *left, const void *right);

// Returns the first matching unsorted-item index through out_index.
bool linear_search(void *const *items, size_t count, const void *key, LinearSearchCompareFn compare, size_t *out_index);
