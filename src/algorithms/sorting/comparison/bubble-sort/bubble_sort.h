// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Compares two caller-owned items using qsort-style ordering.
typedef int (*BubbleSortCompareFn)(const void *left, const void *right);

// Stably sorts item pointers in ascending comparison order.
bool bubble_sort(void **items, size_t count, BubbleSortCompareFn compare);
