# Adjacency Matrix

Dynamic weighted graph representation storing caller-owned values in nodes plus
weighted Edge values in one flat graph-owned matrix. The standard choice for
dense graphs.

## How It Works

The graph owns a growable array of node pointers. Each node holds a
caller-owned value and stable dense internal index. A graph-owned flat
`Edge *edges` allocation stores an Edge at
`from->index * node_capacity + to->index`. An Edge with `target == NULL` marks
an absent connection; a present Edge stores its target and weight. The defining
strength is O(1) edge tests and weight lookup. Undirected graphs update matching
Edge values in both directions. This uses V^2 storage regardless of how few
edges exist, and neighbor iteration scans a full row even for a vertex with one
edge. Dense graphs amortize both; sparse graphs are better served by the
adjacency list.

## Required API

```c
typedef struct AdjacencyMatrix AdjacencyMatrix;
typedef struct Node Node;
typedef struct GraphView GraphView;
typedef bool (*AdjacencyMatrixVisitFn)(Node *neighbor,
                                        uint64_t weight, void *context);

AdjacencyMatrix *adjacency_matrix_create(bool directed);
void adjacency_matrix_destroy(AdjacencyMatrix *graph);
bool adjacency_matrix_add_node(AdjacencyMatrix *graph, void *value,
                                Node **out_node);
bool adjacency_matrix_node_value(const Node *node,
                                  void **out_value);
bool adjacency_matrix_node_at(const AdjacencyMatrix *graph, size_t index,
                               Node **out_node);
bool adjacency_matrix_add_edge(AdjacencyMatrix *graph,
                                Node *from,
                                Node *to,
                                uint64_t weight);
bool adjacency_matrix_remove_edge(AdjacencyMatrix *graph,
                                   Node *from,
                                   Node *to);
bool adjacency_matrix_has_edge(const AdjacencyMatrix *graph,
                                const Node *from,
                                const Node *to);
bool adjacency_matrix_get_edge_weight(const AdjacencyMatrix *graph,
                                       const Node *from,
                                       const Node *to,
                                       uint64_t *out_weight);
bool adjacency_matrix_neighbors(const AdjacencyMatrix *graph,
                                 const Node *node,
                                 AdjacencyMatrixVisitFn visit, void *context);
size_t adjacency_matrix_node_count(const AdjacencyMatrix *graph);
size_t adjacency_matrix_edge_count(const AdjacencyMatrix *graph);
bool adjacency_matrix_graph_view(const AdjacencyMatrix *graph,
                                 GraphView *out_view);
```

The checked-in implementation stores Edge values in a flat matrix and is
covered by focused behavior tests.

## Contract

- `create` starts with zero nodes. `add_node(value)` appends one node, returns
  its stable pointer through `out_node`, and assigns it the next dense internal
  index. The graph never frees caller-owned values.
- Node pointers passed to graph operations must belong to that graph; foreign
  or `NULL` nodes are rejected cleanly.
- Each capacity expansion allocates a larger flat Edge grid and copies active
  Edge values by node index; every existing edge and weight remains attached to the
  same nodes.
- `has_edge(u, v)` is a single cell read — the representation's defining
  strength.
- `add_edge(from, to, weight)` stores a nonnegative weight in
  `edges[from->index * node_capacity + to->index]`. Undirected graphs keep
  matching Edge values symmetric by updating both directions together.
- Adding an existing edge and removing an absent edge return `false` without
  changing graph state.
- Neighbor iteration scans a node's full flat-matrix row, even when it has few
  edges. `destroy` frees graph-owned nodes, the Edge matrix, and node arrays,
  never caller data values.
- BFS and DFS ignore weight; Dijkstra and A* consume it. An unweighted edge
  uses weight `1U`.
- `adjacency_matrix_graph_view` fills a non-owning GraphView adapter that
  exposes shared Node pointers and weighted neighbors without exposing graph
  fields.

## Complexity Targets

- `add_edge`, `remove_edge`, `has_edge`, `get_edge_weight`: O(1)
- Add node: O(N^2) due to matrix resize and copy
- Iterate a node's neighbors: O(N), regardless of degree
- Full traversal of all edges: O(N^2)
- Space: O(N^2), independent of edge count

## Verification

```text
make test NAME=data-structures/graphs/representations/adjacency-matrix
make benchmark NAME=data-structures/graphs/representations/adjacency-matrix BENCHMARK=adjacency_matrix
```

At 1,000 distinct targets per sample on this development machine, measured
medians were 8.50 ns/op for insertion, 2.30 ns/op for edge lookup, and 2.90
ns/op for removal. Matrix construction happens outside the timed operation
loop; these results measure direct cell operations, not O(N^2) capacity growth.
