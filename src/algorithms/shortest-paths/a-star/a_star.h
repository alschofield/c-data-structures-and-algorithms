#ifndef A_STAR_H
#define A_STAR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct WeightedGraphEdge {
    size_t to;
    uint64_t weight;
} WeightedGraphEdge;

typedef struct WeightedGraph {
    size_t vertex_count;
    const WeightedGraphEdge *const *edges;
    const size_t *degrees;
} WeightedGraph;

typedef uint64_t (*AStarHeuristicFn)(size_t vertex, void *context);

bool a_star(const WeightedGraph *graph, size_t source, size_t goal,
            AStarHeuristicFn heuristic, void *context,
            size_t *out_path, size_t *out_path_length);

#endif
