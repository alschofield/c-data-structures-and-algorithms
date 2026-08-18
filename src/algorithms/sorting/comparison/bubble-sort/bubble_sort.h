#ifndef BUBBLE_SORT_H
#define BUBBLE_SORT_H

#include <stdbool.h>
#include <stddef.h>

typedef int (*BubbleSortCompareFn)(const void *left, const void *right);

bool bubble_sort(void **items, size_t count, BubbleSortCompareFn compare);

#endif
