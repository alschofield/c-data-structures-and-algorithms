// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>
// Declares uint64_t.
#include <stdint.h>

#include "../../graph-view/graph_view.h"

// Names the opaque adjacency-list graph.
typedef struct AdjacencyList AdjacencyList;
// Visits one weighted outgoing neighbor and returns false to stop iteration.
typedef bool (*AdjacencyListVisitFn)(Node *neighbor, uint64_t weight, void *context);

// Allocates an empty directed or undirected graph.
AdjacencyList *adjacency_list_create(bool directed);
// Frees graph-owned Nodes and edges without freeing caller-owned values.
void adjacency_list_destroy(AdjacencyList *graph);
// Adds one caller-valued Node and returns its graph-owned handle.
bool adjacency_list_add_node(AdjacencyList *graph, void *value, Node **out_node);
// Returns a Node's caller-owned value pointer.
bool adjacency_list_node_value(const Node *node, void **out_value);
// Returns one graph-owned Node by dense index.
bool adjacency_list_node_at(const AdjacencyList *graph, size_t index, Node **out_node);
// Adds one weighted logical edge, mirroring undirected storage when needed.
bool adjacency_list_add_edge(AdjacencyList *graph, Node *from, Node *to, uint64_t weight);
// Reports whether one directed edge exists.
bool adjacency_list_has_edge(const AdjacencyList *graph, const Node *from, const Node *to);
// Visits one Node's weighted outgoing neighbors.
bool adjacency_list_neighbors(const AdjacencyList *graph, const Node *node, AdjacencyListVisitFn visit, void *context);
// Returns the number of graph-owned Nodes.
size_t adjacency_list_node_count(const AdjacencyList *graph);
// Returns the number of logical public edges.
size_t adjacency_list_edge_count(const AdjacencyList *graph);
// Fills a non-owning GraphView adapter for this graph.
bool adjacency_list_graph_view(const AdjacencyList *graph, GraphView *out_view);
