# Union-Find

An opaque disjoint-set forest over fixed dense indexes. `find` path-compresses parent chains and `union` links roots by rank.

## C API
```c
UnionFind *union_find_create(size_t element_count); void union_find_destroy(UnionFind *set);
bool union_find_find(UnionFind *set, size_t element, size_t *out_representative);
bool union_find_union(UnionFind *set, size_t a, size_t b, bool *out_merged);
bool union_find_connected(UnionFind *set, size_t a, size_t b, bool *out_connected); size_t union_find_set_count(const UnionFind *set);
```

## Behavior, Ownership, and Invariants
- Valid elements are `[0, element_count)` for the lifetime of the set. Creation allocates parent/rank arrays and rejects allocation or byte-size overflow; destroy is null-safe.
- Each root is self-parented. `find` returns a representative and compresses its traversed path. `union` reports successful processing with `out_merged == false` when both elements already share a set; an effective union decrements set count.
- All result outputs are required and left untouched by invalid calls. Null set count is zero.

## Complexity and Verification
Find, union, and connected are amortized O(alpha(n)); space is O(n). Verify with `make test NAME=data-structures/graphs/disjoint-sets/union-find`.
