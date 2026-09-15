# Adjacency Matrix

An opaque weighted graph using a flat, dynamically grown square `Edge` matrix and stable graph-owned node handles. It favors dense graphs and constant-time edge queries.

## C API
```c
AdjacencyMatrix *adjacency_matrix_create(bool directed); void adjacency_matrix_destroy(AdjacencyMatrix *graph);
bool adjacency_matrix_add_node(AdjacencyMatrix *graph, void *value, Node **out_node); bool adjacency_matrix_node_value(const Node *node, void **out_value);
bool adjacency_matrix_node_at(const AdjacencyMatrix *graph, size_t index, Node **out_node); bool adjacency_matrix_add_edge(AdjacencyMatrix *graph, Node *from, Node *to, uint64_t weight);
bool adjacency_matrix_remove_edge(AdjacencyMatrix *graph, Node *from, Node *to); bool adjacency_matrix_has_edge(const AdjacencyMatrix *graph, const Node *from, const Node *to);
bool adjacency_matrix_get_edge_weight(const AdjacencyMatrix *graph, const Node *from, const Node *to, uint64_t *out_weight);
typedef bool (*AdjacencyMatrixVisitFn)(Node *neighbor, uint64_t weight, void *context); bool adjacency_matrix_neighbors(const AdjacencyMatrix *graph, const Node *node, AdjacencyMatrixVisitFn visit, void *context);
size_t adjacency_matrix_node_count(const AdjacencyMatrix *graph); size_t adjacency_matrix_edge_count(const AdjacencyMatrix *graph); bool adjacency_matrix_graph_view(const AdjacencyMatrix *graph, GraphView *out_view);
```

## Behavior, Ownership, and Invariants
- Nodes and matrix storage are graph-owned; values are borrowed. Handles must belong to `graph`; outputs/visitors must be non-null where declared.
- A present matrix slot is one edge. Add rejects an existing direct edge; remove/get reject an absent edge. Undirected graphs mirror add/remove yet count one logical edge.
- Growing checks pointer, squared-cell, and byte-size overflow and prepares replacement allocations before publishing them. The GraphView is borrowed and becomes invalid after graph destruction or structural mutation.

## Complexity and Verification
Edge test/add/remove/weight lookup are O(1); neighbor iteration O(V); node growth copies O(V^2); space O(V^2). Verify with `make test NAME=data-structures/graphs/representations/adjacency-matrix`.
