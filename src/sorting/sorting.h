#ifndef SORTING_H
#define SORTING_H

#include <stdbool.h>
#include <stddef.h>

typedef int (*SortCompareFn)(const void *left, const void *right);

bool bubble_sort(void **items, size_t count, SortCompareFn compare);
bool insertion_sort(void **items, size_t count, SortCompareFn compare);
bool selection_sort(void **items, size_t count, SortCompareFn compare);
bool merge_sort(void **items, size_t count, SortCompareFn compare);
bool quick_sort(void **items, size_t count, SortCompareFn compare);

#endif
