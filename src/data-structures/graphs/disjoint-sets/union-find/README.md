# Union-Find

Disjoint-set forest over dense integer elements supporting near-constant-time
set merging and membership queries.

## How It Works

Answers one question fast while groups keep merging: are these two elements
in the same group? Every element points at a parent; following parents ends
at the group's representative, and two elements share a group exactly when
they reach the same representative. Merging groups means pointing one
representative at the other.

Left alone, parent chains grow long, so two cheap tricks keep the forest
near-flat. Path compression: after walking up to find the root, re-point
every visited node directly at it — the lookup flattens the tree behind
itself (which is why find takes a mutable structure). Union by rank: attach
the shorter tree under the taller root so chains grow only when unavoidable.
Together they make operations effectively constant time.

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

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

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

## GraphView Integration

Union-find remains representation-independent and stores only dense integer
elements; it does not own or depend on a GraphView. Graph algorithms map a
GraphView Node to its stable `Node.index`, then pass that index to union-find.
Kruskal uses `graph_view_vertex_count` to size the disjoint set and
`graph_view_node_at` plus neighbor iteration to enumerate graph edges. This
keeps union-find reusable for non-graph grouping problems while letting graph
algorithms use either adjacency-list or adjacency-matrix views.

## Complexity Targets

- `find`, `union`, `connected`: amortized O(alpha(n)) with path compression
  plus union by rank — effectively constant for all practical n
- `create`: O(n)
- Space: O(n) for parent and rank arrays
