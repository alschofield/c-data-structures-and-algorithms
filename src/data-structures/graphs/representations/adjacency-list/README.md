# Adjacency List

Dynamic weighted graph representation storing caller-owned values in nodes and
outgoing edges on each node. The standard choice for sparse graphs and the
shared input for every graph algorithm in this curriculum.

## How It Works

The graph owns a growable array of node pointers. Each node holds a
caller-owned value, a stable dense internal index, and a growable list of
outgoing edges; each edge points to its target node and stores a nonnegative
weight. Memory scales with what actually exists — nodes plus edges — which
makes this the default representation for sparse graphs, and neighbor
iteration (the operation traversals live on) touches exactly the edges that
exist. The trade: asking
"does u point at v?" means scanning u's bucket, O(deg(u)). Pairing rule of
thumb: adjacency list for sparse graphs and traversal-heavy work, adjacency
matrix for dense graphs and constant-time edge tests.

## Required API

```c
typedef struct AdjacencyList AdjacencyList;
typedef struct Node Node;
typedef struct GraphView GraphView;
typedef bool (*AdjacencyListVisitFn)(Node *neighbor,
                                     uint64_t weight, void *context);

AdjacencyList *adjacency_list_create(bool directed);
void adjacency_list_destroy(AdjacencyList *graph);
bool adjacency_list_add_node(AdjacencyList *graph, void *value,
                             Node **out_node);
bool adjacency_list_node_value(const Node *node, void **out_value);
bool adjacency_list_node_at(const AdjacencyList *graph, size_t index,
                            Node **out_node);
bool adjacency_list_add_edge(AdjacencyList *graph, Node *from,
                             Node *to,
                             uint64_t weight);
bool adjacency_list_has_edge(const AdjacencyList *graph,
                             const Node *from,
                             const Node *to);
bool adjacency_list_neighbors(const AdjacencyList *graph,
                              const Node *node,
                              AdjacencyListVisitFn visit, void *context);
size_t adjacency_list_node_count(const AdjacencyList *graph);
size_t adjacency_list_edge_count(const AdjacencyList *graph);
bool adjacency_list_graph_view(const AdjacencyList *graph, GraphView *out_view);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

## Contract

- `create` starts with zero nodes. `add_node(value)` appends one node, returns
  its stable pointer through `out_node`, and assigns it the next dense internal
  index. The graph never frees caller-owned values.
- Node pointers passed to graph operations must belong to that graph; foreign
  or `NULL` nodes are rejected cleanly.
- `add_edge(from, to, weight)` appends a nonnegative weighted edge to from's
  list; the directed/undirected policy is fixed at creation, and undirected
  graphs store matching weighted edges in both directions consistently.
- Duplicate edge policy is explicit: either reject duplicates or document
  multigraph behavior; `has_edge` scans only u's list.
- Neighbor iteration visits exactly a node's out-edges, each once, with target
  node pointers and weights, in a deterministic order.
- Self-loops are permitted unless documented otherwise.
- `destroy` frees all graph-owned nodes, edge arrays, and the node array; it
  never frees caller data values.
- BFS and DFS ignore weight; Dijkstra and A* consume it. An unweighted edge
  uses weight `1U`.
- `adjacency_list_graph_view` fills a non-owning GraphView adapter that maps
  each internal node index to weighted neighbor indexes, letting algorithms
  traverse this representation without depending on its fields.

## Complexity Targets

- `add_node`, `add_edge`: O(1) amortized
- `has_edge(from, to)`: O(deg(from))
- Iterate a node's neighbors: O(deg(node))
- Full traversal of all edges: O(V + E)
- Space: O(V + E)
