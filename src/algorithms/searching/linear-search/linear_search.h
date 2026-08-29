#include <stdbool.h>
#include <stddef.h>

typedef int (*LinearSearchCompareFn)(const void *left, const void *right);

bool linear_search(void *const *items, size_t count, const void *key, LinearSearchCompareFn compare, size_t *out_index);
