#include <stdbool.h>
#include <stddef.h>

typedef int (*MergeSortCompareFn)(const void *left, const void *right);

bool merge_sort(void **items, size_t count, MergeSortCompareFn compare);
