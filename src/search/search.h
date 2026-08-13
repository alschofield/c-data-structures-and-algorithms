#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>
#include <stddef.h>

typedef int (*SearchCompareFn)(const void *left, const void *right);

bool linear_search(
    void *const *items,
    size_t count,
    const void *key,
    SearchCompareFn compare,
    size_t *out_index
);
bool binary_search(
    void *const *items,
    size_t count,
    const void *key,
    SearchCompareFn compare,
    size_t *out_index
);

#endif
