#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <stdbool.h>
#include <stddef.h>

typedef struct UnionFind UnionFind;

UnionFind *union_find_create(size_t element_count);
void union_find_destroy(UnionFind *set);
bool union_find_find(UnionFind *set, size_t element, size_t *out_representative);
bool union_find_union(UnionFind *set, size_t a, size_t b, bool *out_merged);
bool union_find_connected(UnionFind *set, size_t a, size_t b, bool *out_connected);
size_t union_find_set_count(const UnionFind *set);

#endif
