# Union-Find

Disjoint-set forest over dense integer elements supporting near-constant-time
set merging and membership queries.

## Required API

```c
bool union_find_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that. The contract below specifies the structure and
operations (create/destroy for n elements, find, union, connected, set count).

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

## Learning Focus

Union-find delivers one of the most striking results in data structures: two
small optimizations turn a potentially linear operation into inverse-Ackermann
amortized time. Implementing it teaches amortized reasoning on a structure
that mutates during reads (path compression), and it is the enabling primitive
for Kruskal's MST, connectivity queries, and cycle detection in undirected
graphs.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
