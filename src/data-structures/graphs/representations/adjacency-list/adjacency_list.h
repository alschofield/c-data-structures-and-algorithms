#ifndef ADJACENCY_LIST_H
#define ADJACENCY_LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct AdjacencyList AdjacencyList;
typedef bool (*AdjacencyListVisitFn)(size_t neighbor, void *context);

AdjacencyList *adjacency_list_create(size_t vertex_count, bool directed);
void adjacency_list_destroy(AdjacencyList *graph);
bool adjacency_list_add_edge(AdjacencyList *graph, size_t from, size_t to);
bool adjacency_list_has_edge(const AdjacencyList *graph, size_t from, size_t to);
bool adjacency_list_neighbors(const AdjacencyList *graph, size_t vertex,
                              AdjacencyListVisitFn visit, void *context);
size_t adjacency_list_vertex_count(const AdjacencyList *graph);
size_t adjacency_list_edge_count(const AdjacencyList *graph);

#endif
