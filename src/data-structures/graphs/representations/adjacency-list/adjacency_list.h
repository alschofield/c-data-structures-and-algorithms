#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../graph-view/graph_view.h"

typedef struct AdjacencyList AdjacencyList;
typedef bool (*AdjacencyListVisitFn)(Node *neighbor, uint64_t weight, void *context);

AdjacencyList *adjacency_list_create(bool directed);
void adjacency_list_destroy(AdjacencyList *graph);
bool adjacency_list_add_node(AdjacencyList *graph, void *value, Node **out_node);
bool adjacency_list_node_value(const Node *node, void **out_value);
bool adjacency_list_node_at(const AdjacencyList *graph, size_t index, Node **out_node);
bool adjacency_list_add_edge(AdjacencyList *graph, Node *from, Node *to, uint64_t weight);
bool adjacency_list_has_edge(const AdjacencyList *graph, const Node *from, const Node *to);
bool adjacency_list_neighbors(const AdjacencyList *graph, const Node *node, AdjacencyListVisitFn visit, void *context);
size_t adjacency_list_node_count(const AdjacencyList *graph);
size_t adjacency_list_edge_count(const AdjacencyList *graph);
bool adjacency_list_graph_view(const AdjacencyList *graph, GraphView *out_view);
