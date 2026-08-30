// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Compares two caller-owned items using qsort-style ordering.
typedef int (*SelectionSortCompareFn)(const void *left, const void *right);

// Sorts item pointers in ascending comparison order by repeated selection.
bool selection_sort(void **items, size_t count, SelectionSortCompareFn compare);
