# Adjacency List

An opaque weighted graph with a dynamic node array and a dynamic outgoing-edge collection per node. It is suited to sparse, traversal-heavy graphs.

## C API
```c
AdjacencyList *adjacency_list_create(bool directed); void adjacency_list_destroy(AdjacencyList *graph);
bool adjacency_list_add_node(AdjacencyList *graph, void *value, Node **out_node); bool adjacency_list_node_value(const Node *node, void **out_value);
bool adjacency_list_node_at(const AdjacencyList *graph, size_t index, Node **out_node); bool adjacency_list_add_edge(AdjacencyList *graph, Node *from, Node *to, uint64_t weight);
bool adjacency_list_has_edge(const AdjacencyList *graph, const Node *from, const Node *to); typedef bool (*AdjacencyListVisitFn)(Node *neighbor, uint64_t weight, void *context);
bool adjacency_list_neighbors(const AdjacencyList *graph, const Node *node, AdjacencyListVisitFn visit, void *context); size_t adjacency_list_node_count(const AdjacencyList *graph);
size_t adjacency_list_edge_count(const AdjacencyList *graph); bool adjacency_list_graph_view(const AdjacencyList *graph, GraphView *out_view);
```

## Behavior, Ownership, and Invariants
- Nodes/edges are graph-owned, have stable dense node indexes, and reject foreign handles. Values are borrowed; destruction is null-safe and never frees values.
- Directed insertion adds/updates one arc. Undirected insertion mirrors non-self-loop storage but counts it as one logical edge; its rollback preserves state if the mirror cannot be added. Re-adding an edge updates weight and does not increase edge count.
- Outputs/visitors are required where declared. The GraphView is borrowed and exposes outgoing edges by dense target index. Capacity/byte-size overflow and allocation failure fail without partially adding a logical node/edge.

## Complexity and Verification
Node append is amortized O(1); add-edge and has-edge are O(out-degree); neighbor iteration O(out-degree); space O(V + E). Verify with `make test NAME=data-structures/graphs/representations/adjacency-list`.
