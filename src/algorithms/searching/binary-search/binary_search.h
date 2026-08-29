#include <stdbool.h>
#include <stddef.h>

typedef int (*BinarySearchCompareFn)(const void *left, const void *right);

bool binary_search(void *const *items, size_t count, const void *key, BinarySearchCompareFn compare, size_t *out_index);
