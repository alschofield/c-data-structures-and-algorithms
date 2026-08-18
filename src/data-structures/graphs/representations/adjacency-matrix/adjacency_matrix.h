#ifndef ADJACENCY_MATRIX_H
#define ADJACENCY_MATRIX_H

#include <stdbool.h>
#include <stddef.h>

typedef struct AdjacencyMatrix AdjacencyMatrix;
typedef bool (*AdjacencyMatrixVisitFn)(size_t neighbor, void *context);

AdjacencyMatrix *adjacency_matrix_create(size_t vertex_count, bool directed);
void adjacency_matrix_destroy(AdjacencyMatrix *graph);
bool adjacency_matrix_add_edge(AdjacencyMatrix *graph, size_t from, size_t to);
bool adjacency_matrix_remove_edge(AdjacencyMatrix *graph, size_t from, size_t to);
bool adjacency_matrix_has_edge(const AdjacencyMatrix *graph, size_t from, size_t to);
bool adjacency_matrix_neighbors(const AdjacencyMatrix *graph, size_t vertex,
                                AdjacencyMatrixVisitFn visit, void *context);
size_t adjacency_matrix_vertex_count(const AdjacencyMatrix *graph);
size_t adjacency_matrix_edge_count(const AdjacencyMatrix *graph);

#endif
