// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Names the opaque disjoint-set forest.
typedef struct UnionFind UnionFind;

// Allocates one singleton set for every dense element index.
UnionFind *union_find_create(size_t element_count);
// Frees union-find parent and rank arrays.
void union_find_destroy(UnionFind *set);
// Returns the root representative for one dense element.
bool union_find_find(UnionFind *set, size_t element, size_t *out_representative);
// Merges two element sets and reports whether the forest changed.
bool union_find_union(UnionFind *set, size_t a, size_t b, bool *out_merged);
// Reports whether two elements share a root representative.
bool union_find_connected(UnionFind *set, size_t a, size_t b, bool *out_connected);
// Returns the current number of disconnected sets.
size_t union_find_set_count(const UnionFind *set);
