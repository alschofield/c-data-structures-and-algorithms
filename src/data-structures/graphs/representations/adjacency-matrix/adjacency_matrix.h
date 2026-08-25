#include <stdbool.h>
#include <stddef.h>

#include "../../graph-view/graph_view.h"

typedef struct Edge Edge;
typedef struct Node Node;
typedef struct AdjacencyMatrix AdjacencyMatrix;
typedef bool (*AdjacencyMatrixVisitFn)(Node *neighbor, uint64_t weight, void *context);

AdjacencyMatrix *adjacency_matrix_create(bool directed);
void adjacency_matrix_destroy(AdjacencyMatrix *graph);
bool adjacency_matrix_add_node(AdjacencyMatrix *graph, void *value, Node **out_node);
bool adjacency_matrix_node_value(const Node *node, void **out_value);
bool adjacency_matrix_node_at(const AdjacencyMatrix *graph, size_t index, Node **out_node);
bool adjacency_matrix_add_edge(AdjacencyMatrix *graph, Node *from, Node *to, uint64_t weight);
bool adjacency_matrix_remove_edge(AdjacencyMatrix *graph, Node *from, Node *to);
bool adjacency_matrix_has_edge(const AdjacencyMatrix *graph, const Node *from, const Node *to);
bool adjacency_matrix_get_edge_weight(const AdjacencyMatrix *graph, const Node *from, const Node *to, uint64_t *out_weight);
bool adjacency_matrix_neighbors(const AdjacencyMatrix *graph, const Node *node, AdjacencyMatrixVisitFn visit, void *context);
size_t adjacency_matrix_node_count(const AdjacencyMatrix *graph);
size_t adjacency_matrix_edge_count(const AdjacencyMatrix *graph);
bool adjacency_matrix_graph_view(const AdjacencyMatrix *graph, GraphView *out_view);
