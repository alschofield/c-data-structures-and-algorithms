// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

#include "../../graph-view/graph_view.h"

// Names the opaque adjacency-matrix graph.
typedef struct AdjacencyMatrix AdjacencyMatrix;
// Visits one weighted outgoing neighbor and returns false to stop iteration.
typedef bool (*AdjacencyMatrixVisitFn)(Node *neighbor, uint64_t weight, void *context);

// Allocates an empty directed or undirected matrix graph.
AdjacencyMatrix *adjacency_matrix_create(bool directed);
// Frees graph-owned Nodes and flat Edge matrix storage.
void adjacency_matrix_destroy(AdjacencyMatrix *graph);
// Adds one caller-valued Node and returns its graph-owned handle.
bool adjacency_matrix_add_node(AdjacencyMatrix *graph, void *value, Node **out_node);
// Returns a Node's caller-owned value pointer.
bool adjacency_matrix_node_value(const Node *node, void **out_value);
// Returns one graph-owned Node by dense index.
bool adjacency_matrix_node_at(const AdjacencyMatrix *graph, size_t index, Node **out_node);
// Adds one weighted logical edge, mirroring undirected storage when needed.
bool adjacency_matrix_add_edge(AdjacencyMatrix *graph, Node *from, Node *to, uint64_t weight);
// Removes one logical edge and any undirected mirror.
bool adjacency_matrix_remove_edge(AdjacencyMatrix *graph, Node *from, Node *to);
// Reports whether one direct Edge matrix slot is present.
bool adjacency_matrix_has_edge(const AdjacencyMatrix *graph, const Node *from, const Node *to);
// Returns one direct Edge matrix weight.
bool adjacency_matrix_get_edge_weight(const AdjacencyMatrix *graph, const Node *from, const Node *to, uint64_t *out_weight);
// Visits one Node's weighted outgoing neighbors.
bool adjacency_matrix_neighbors(const AdjacencyMatrix *graph, const Node *node, AdjacencyMatrixVisitFn visit, void *context);
// Returns the number of graph-owned Nodes.
size_t adjacency_matrix_node_count(const AdjacencyMatrix *graph);
// Returns the number of logical public edges.
size_t adjacency_matrix_edge_count(const AdjacencyMatrix *graph);
// Fills a non-owning GraphView adapter for this graph.
bool adjacency_matrix_graph_view(const AdjacencyMatrix *graph, GraphView *out_view);
