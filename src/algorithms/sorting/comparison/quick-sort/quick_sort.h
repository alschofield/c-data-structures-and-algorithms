#ifndef QUICK_SORT_H
#define QUICK_SORT_H

#include <stdbool.h>
#include <stddef.h>

typedef int (*QuickSortCompareFn)(const void *left, const void *right);

bool quick_sort(void **items, size_t count, QuickSortCompareFn compare);

#endif
