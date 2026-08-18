#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DIJKSTRA_INFINITY UINT64_MAX
#define DIJKSTRA_NO_PARENT SIZE_MAX

typedef struct WeightedGraphEdge {
    size_t to;
    uint64_t weight;
} WeightedGraphEdge;

typedef struct WeightedGraph {
    size_t vertex_count;
    const WeightedGraphEdge *const *edges;
    const size_t *degrees;
} WeightedGraph;

bool dijkstra(const WeightedGraph *graph, size_t source,
              uint64_t *out_distances, size_t *out_parents);

#endif
