#ifndef HEAP_SORT_H
#define HEAP_SORT_H

#include <stdbool.h>
#include <stddef.h>

typedef int (*HeapSortCompareFn)(const void *left, const void *right);

bool heap_sort(void **items, size_t count, HeapSortCompareFn compare);

#endif
