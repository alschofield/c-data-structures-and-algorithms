// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Compares two caller-owned items using qsort-style ordering.
typedef int (*InsertionSortCompareFn)(const void *left, const void *right);

// Stably sorts item pointers in ascending comparison order by insertion.
bool insertion_sort(void **items, size_t count, InsertionSortCompareFn compare);
