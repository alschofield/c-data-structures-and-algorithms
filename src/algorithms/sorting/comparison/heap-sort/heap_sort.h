// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Compares two caller-owned items using qsort-style ordering.
typedef int (*HeapSortCompareFn)(const void *left, const void *right);

// Sorts item pointers in place using a max-heap extraction phase.
bool heap_sort(void **items, size_t count, HeapSortCompareFn compare);
