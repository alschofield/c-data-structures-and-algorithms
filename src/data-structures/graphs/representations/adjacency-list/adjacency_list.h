#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../graph-view/graph_view.h"

typedef struct Edge Edge;
typedef struct AdjacencyList AdjacencyList;
typedef struct AdjacencyListNode AdjacencyListNode;
typedef bool (*AdjacencyListVisitFn)(AdjacencyListNode *neighbor, uint64_t weight, void *context);

AdjacencyList *adjacency_list_create(bool directed);
void adjacency_list_destroy(AdjacencyList *graph);
bool adjacency_list_add_node(AdjacencyList *graph, void *value, AdjacencyListNode **out_node);
bool adjacency_list_node_value(const AdjacencyListNode *node, void **out_value);
bool adjacency_list_node_at(const AdjacencyList *graph, size_t index, AdjacencyListNode **out_node);
bool adjacency_list_add_edge(AdjacencyList *graph, AdjacencyListNode *from, AdjacencyListNode *to, uint64_t weight);
bool adjacency_list_has_edge(const AdjacencyList *graph, const AdjacencyListNode *from, const AdjacencyListNode *to);
bool adjacency_list_neighbors(const AdjacencyList *graph, const AdjacencyListNode *node, AdjacencyListVisitFn visit, void *context);
size_t adjacency_list_node_count(const AdjacencyList *graph);
size_t adjacency_list_edge_count(const AdjacencyList *graph);
bool adjacency_list_graph_view(const AdjacencyList *graph, GraphView *out_view);
