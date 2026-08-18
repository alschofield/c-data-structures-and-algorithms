// Scaffold stub: replace the bodies with the real implementation.
#include "adjacency_matrix.h"

AdjacencyMatrix *adjacency_matrix_create(size_t vertex_count, bool directed) {
    (void)vertex_count;
    (void)directed;
    return NULL;
}

void adjacency_matrix_destroy(AdjacencyMatrix *graph) {
    (void)graph;
}

bool adjacency_matrix_add_edge(AdjacencyMatrix *graph, size_t from, size_t to) {
    (void)graph;
    (void)from;
    (void)to;
    return false;
}

bool adjacency_matrix_remove_edge(AdjacencyMatrix *graph, size_t from, size_t to) {
    (void)graph;
    (void)from;
    (void)to;
    return false;
}

bool adjacency_matrix_has_edge(const AdjacencyMatrix *graph, size_t from, size_t to) {
    (void)graph;
    (void)from;
    (void)to;
    return false;
}

bool adjacency_matrix_neighbors(const AdjacencyMatrix *graph, size_t vertex,
                                AdjacencyMatrixVisitFn visit, void *context) {
    (void)graph;
    (void)vertex;
    (void)visit;
    (void)context;
    return false;
}

size_t adjacency_matrix_vertex_count(const AdjacencyMatrix *graph) {
    (void)graph;
    return 0U;
}

size_t adjacency_matrix_edge_count(const AdjacencyMatrix *graph) {
    (void)graph;
    return 0U;
}
