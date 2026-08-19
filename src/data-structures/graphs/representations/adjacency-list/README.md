# Adjacency List

Graph representation storing, for each vertex, a list of its outgoing edges.
The standard choice for sparse graphs.

## How It Works

One bucket per vertex, listing the vertices it points at. Memory scales with
what actually exists — vertices plus edges — which makes this the default
representation for sparse graphs, and neighbor iteration (the operation
traversals live on) touches exactly the edges that exist. The trade: asking
"does u point at v?" means scanning u's bucket, O(deg(u)). Pairing rule of
thumb: adjacency list for sparse graphs and traversal-heavy work, adjacency
matrix for dense graphs and constant-time edge tests.

## Required API

```c
typedef struct AdjacencyList AdjacencyList;
typedef bool (*AdjacencyListVisitFn)(size_t neighbor, void *context);

AdjacencyList *adjacency_list_create(size_t vertex_count, bool directed);
void adjacency_list_destroy(AdjacencyList *graph);
bool adjacency_list_add_edge(AdjacencyList *graph, size_t from, size_t to);
bool adjacency_list_has_edge(const AdjacencyList *graph, size_t from, size_t to);
bool adjacency_list_neighbors(const AdjacencyList *graph, size_t vertex,
                              AdjacencyListVisitFn visit, void *context);
size_t adjacency_list_vertex_count(const AdjacencyList *graph);
size_t adjacency_list_edge_count(const AdjacencyList *graph);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

## Contract

- Vertexes are dense indexes `[0, vertex_count)`; out-of-range vertexes are
  rejected cleanly by every operation.
- `add_edge(u, v)` appends v to u's edge list; the directed/undirected policy
  is fixed at creation, and undirected graphs store both directions
  consistently.
- Duplicate edge policy is explicit: either reject duplicates or document
  multigraph behavior; `has_edge` scans only u's list.
- Neighbor iteration visits exactly u's out-edges, each once, in a
  deterministic order.
- Self-loops are permitted unless documented otherwise.
- `destroy` frees all list nodes and the vertex array; the structure owns its
  edges, never caller data.

## Complexity Targets

- `add_edge`: O(1) (amortized, prepend or tail append)
- `has_edge(u, v)`: O(deg(u))
- Iterate neighbors of u: O(deg(u))
- Full traversal of all edges: O(V + E)
- Space: O(V + E)
