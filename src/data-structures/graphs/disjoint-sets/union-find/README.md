# Union-Find

Disjoint-set forest over dense integer elements supporting near-constant-time
set merging and membership queries.

## Required API

```c
typedef struct UnionFind UnionFind;

UnionFind *union_find_create(size_t element_count);
void union_find_destroy(UnionFind *set);
bool union_find_find(UnionFind *set, size_t element, size_t *out_representative);
bool union_find_union(UnionFind *set, size_t a, size_t b, bool *out_merged);
bool union_find_connected(UnionFind *set, size_t a, size_t b, bool *out_connected);
size_t union_find_set_count(const UnionFind *set);
```

The checked-in source is still the scaffold gate `bool
union_find_is_implemented(void)`, which returns `false`; the test asserts exactly that.

## Contract

- Elements are dense indexes `[0, n)`, each initially its own singleton set;
  out-of-range elements are rejected cleanly.
- `find` returns the set representative and applies path compression, so
  repeated finds flatten the tree.
- `union` links by rank (or size): the shorter tree attaches under the taller
  root. Unioning two elements already in the same set is a no-op and must not
  change ranks or the set count.
- `connected(a, b)` is equivalent to `find(a) == find(b)`.
- The representative may change across unions; callers may rely only on
  representative equality within a set, never on a specific value.
- The tracked set count starts at n and decreases by exactly one per
  effective union.

## Complexity Targets

- `find`, `union`, `connected`: amortized O(alpha(n)) with path compression
  plus union by rank — effectively constant for all practical n
- `create`: O(n)
- Space: O(n) for parent and rank arrays
