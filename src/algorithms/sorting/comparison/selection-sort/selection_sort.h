#include <stdbool.h>
#include <stddef.h>

typedef int (*SelectionSortCompareFn)(const void *left, const void *right);

bool selection_sort(void **items, size_t count, SelectionSortCompareFn compare);
