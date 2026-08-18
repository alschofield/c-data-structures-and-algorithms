#include <stdbool.h>
#include <stddef.h>

typedef int (*BubbleSortCompareFn)(const void *left, const void *right);

bool bubble_sort(void **items, size_t count, BubbleSortCompareFn compare);
