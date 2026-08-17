# Adjacency Matrix

Graph representation storing edge presence (or weight) in a V x V grid,
indexed by vertex pair. The standard choice for dense graphs.

## Required API

```c
bool adjacency_matrix_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that. The contract below specifies the graph type and
operations (create/destroy for a fixed vertex count, add_edge, remove_edge,
has_edge, neighbor iteration, vertex/edge counts).

## Contract

- Vertexes are dense indexes `[0, vertex_count)`; out-of-range vertexes are
  rejected cleanly by every operation.
- The matrix is a single contiguous allocation indexed as
  `matrix[u * vertex_count + v]`; a fresh graph has every cell cleared.
- `has_edge(u, v)` is a single cell read — the representation's defining
  strength.
- Undirected graphs keep the matrix symmetric: `add_edge`/`remove_edge`
  update both `(u, v)` and `(v, u)` together.
- Adding an existing edge and removing an absent edge are clean no-ops with
  documented return values.
- Neighbor iteration scans row u in full, even when the vertex has few edges.
- `destroy` frees the matrix storage only.

## Complexity Targets

- `add_edge`, `remove_edge`, `has_edge`: O(1)
- Iterate neighbors of u: O(V), regardless of degree
- Full traversal of all edges: O(V^2)
- Space: O(V^2), independent of edge count

## Learning Focus

The matrix is the memory-for-speed extreme of graph representation: O(1) edge
checks bought with O(V^2) space and O(V) neighbor scans. Implementing both
representations against the same conceptual interface teaches how
representation choice changes algorithm cost — BFS on a matrix is O(V^2), not
O(V + E) — and where each earns its keep (dense graphs, edge-heavy queries,
Floyd-Warshall).

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
