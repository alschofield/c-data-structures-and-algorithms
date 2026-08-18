// Scaffold stub: replace the bodies with the real implementation.
#include "adjacency_list.h"

AdjacencyList *adjacency_list_create(size_t vertex_count, bool directed) {
    (void)vertex_count;
    (void)directed;
    return NULL;
}

void adjacency_list_destroy(AdjacencyList *graph) {
    (void)graph;
}

bool adjacency_list_add_edge(AdjacencyList *graph, size_t from, size_t to) {
    (void)graph;
    (void)from;
    (void)to;
    return false;
}

bool adjacency_list_has_edge(const AdjacencyList *graph, size_t from, size_t to) {
    (void)graph;
    (void)from;
    (void)to;
    return false;
}

bool adjacency_list_neighbors(const AdjacencyList *graph, size_t vertex,
                              AdjacencyListVisitFn visit, void *context) {
    (void)graph;
    (void)vertex;
    (void)visit;
    (void)context;
    return false;
}

size_t adjacency_list_vertex_count(const AdjacencyList *graph) {
    (void)graph;
    return 0U;
}

size_t adjacency_list_edge_count(const AdjacencyList *graph) {
    (void)graph;
    return 0U;
}
