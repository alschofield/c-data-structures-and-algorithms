# Adjacency Matrix

Graph representation storing edge presence (or weight) in a V x V grid,
indexed by vertex pair. The standard choice for dense graphs.

## How It Works

A V x V grid of edge flags flattened into one allocation, indexed as
u * vertex_count + v. Its defining strength is the O(1) edge test — one cell
read answers "does u point at v?" — and undirected graphs simply keep the
matrix symmetric by writing both (u, v) and (v, u). The costs are structural:
V^2 memory regardless of how few edges exist, and neighbor iteration scans a
full row even for a vertex with one edge. Dense graphs amortize both;
sparse graphs are better served by the adjacency list.

## Required API

```c
typedef struct AdjacencyMatrix AdjacencyMatrix;
typedef bool (*AdjacencyMatrixVisitFn)(size_t neighbor, void *context);

AdjacencyMatrix *adjacency_matrix_create(size_t vertex_count, bool directed);
void adjacency_matrix_destroy(AdjacencyMatrix *graph);
bool adjacency_matrix_add_edge(AdjacencyMatrix *graph, size_t from, size_t to);
bool adjacency_matrix_remove_edge(AdjacencyMatrix *graph, size_t from, size_t to);
bool adjacency_matrix_has_edge(const AdjacencyMatrix *graph, size_t from, size_t to);
bool adjacency_matrix_neighbors(const AdjacencyMatrix *graph, size_t vertex,
                                AdjacencyMatrixVisitFn visit, void *context);
size_t adjacency_matrix_vertex_count(const AdjacencyMatrix *graph);
size_t adjacency_matrix_edge_count(const AdjacencyMatrix *graph);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

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
