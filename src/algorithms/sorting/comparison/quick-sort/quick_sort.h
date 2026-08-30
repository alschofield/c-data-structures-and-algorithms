// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Compares two caller-owned items using qsort-style ordering.
typedef int (*QuickSortCompareFn)(const void *left, const void *right);

// Sorts item pointers in place by recursively partitioning around pivots.
bool quick_sort(void **items, size_t count, QuickSortCompareFn compare);
