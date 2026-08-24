# Adjacency Matrix

Dynamic weighted graph representation storing caller-owned values in nodes plus
edge presence and nonnegative weight in a growable N x N grid. The standard
choice for dense graphs.

## How It Works

The graph owns a growable array of node pointers. Each node holds a
caller-owned value and stable dense internal index. Edge flags and weights live
in growable contiguous N x N allocations indexed as u * node_count + v. The
defining strength is the O(1) edge test and weight lookup — one cell answers
"does u point at v, and what does it cost?" — and undirected graphs keep both
V^2 memory regardless of how few edges exist, and neighbor iteration scans a
full row even for a vertex with one edge. Dense graphs amortize both;
sparse graphs are better served by the adjacency list.

## Required API

```c
typedef struct AdjacencyMatrix AdjacencyMatrix;
typedef struct AdjacencyMatrixNode AdjacencyMatrixNode;
typedef struct GraphView GraphView;
typedef bool (*AdjacencyMatrixVisitFn)(AdjacencyMatrixNode *neighbor,
                                       uint64_t weight, void *context);

AdjacencyMatrix *adjacency_matrix_create(bool directed);
void adjacency_matrix_destroy(AdjacencyMatrix *graph);
bool adjacency_matrix_add_node(AdjacencyMatrix *graph, void *value,
                               AdjacencyMatrixNode **out_node);
bool adjacency_matrix_node_value(const AdjacencyMatrixNode *node,
                                 void **out_value);
bool adjacency_matrix_node_at(const AdjacencyMatrix *graph, size_t index,
                              AdjacencyMatrixNode **out_node);
bool adjacency_matrix_add_edge(AdjacencyMatrix *graph,
                               AdjacencyMatrixNode *from,
                               AdjacencyMatrixNode *to,
                               uint64_t weight);
bool adjacency_matrix_remove_edge(AdjacencyMatrix *graph,
                                  AdjacencyMatrixNode *from,
                                  AdjacencyMatrixNode *to);
bool adjacency_matrix_has_edge(const AdjacencyMatrix *graph,
                               const AdjacencyMatrixNode *from,
                               const AdjacencyMatrixNode *to);
bool adjacency_matrix_get_edge_weight(const AdjacencyMatrix *graph,
                                      const AdjacencyMatrixNode *from,
                                      const AdjacencyMatrixNode *to,
                                      uint64_t *out_weight);
bool adjacency_matrix_neighbors(const AdjacencyMatrix *graph,
                                const AdjacencyMatrixNode *node,
                                AdjacencyMatrixVisitFn visit, void *context);
size_t adjacency_matrix_node_count(const AdjacencyMatrix *graph);
size_t adjacency_matrix_edge_count(const AdjacencyMatrix *graph);
bool adjacency_matrix_graph_view(const AdjacencyMatrix *graph,
                                 GraphView *out_view);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

## Contract

- `create` starts with zero nodes. `add_node(value)` appends one node, returns
  its stable pointer through `out_node`, and assigns it the next dense internal
  index. The graph never frees caller-owned values.
- Node pointers passed to graph operations must belong to that graph; foreign
  or `NULL` nodes are rejected cleanly.
- Each node addition grows and reindexes the contiguous presence and weight
  grids; every existing edge and weight remains attached to the same nodes.
- `has_edge(u, v)` is a single cell read — the representation's defining
  strength.
- `add_edge(from, to, weight)` stores a nonnegative weight. Undirected graphs
  keep both presence and weight grids symmetric by updating matching node-index
  cells together.
- Adding an existing edge and removing an absent edge are clean no-ops with
  documented return values.
- Neighbor iteration scans a node's full matrix row, even when it has few
  edges. `destroy` frees graph-owned nodes, matrices, and node arrays, never
  caller data values.
- BFS and DFS ignore weight; Dijkstra and A* consume it. An unweighted edge
  uses weight `1U`.
- `adjacency_matrix_graph_view` fills a non-owning GraphView adapter that maps
  internal node indexes to weighted neighbor indexes, letting algorithms
  traverse this representation without depending on its fields.

## Complexity Targets

- `add_edge`, `remove_edge`, `has_edge`, `get_edge_weight`: O(1)
- Add node: O(N^2) due to matrix resize and copy
- Iterate a node's neighbors: O(N), regardless of degree
- Full traversal of all edges: O(N^2)
- Space: O(N^2), independent of edge count
