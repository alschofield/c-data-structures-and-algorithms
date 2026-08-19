#include <stdbool.h>
#include <stddef.h>

typedef int (*InsertionSortCompareFn)(const void *left, const void *right);

bool insertion_sort(void **items, size_t count, InsertionSortCompareFn compare);
