#ifndef A_STAR_H
#define A_STAR_H

// Declares GraphView, Node, uint64_t, and size_t.
#include "../../../data-structures/graphs/graph-view/graph_view.h"

// Names the private state shared with the neighbor-relaxation callback.
typedef struct AStarHeapContext AStarHeapContext;
// Names one private min-heap proposal.
typedef struct AStarNode AStarNode;

// Estimates the remaining goal distance from one graph Node index.
typedef uint64_t (*AStarHeuristicFn)(size_t vertex, void *context);

// Finds an optimal source-to-goal path in a nonnegative weighted graph.
bool a_star(const GraphView *graph, Node *source, Node *goal,
            AStarHeuristicFn heuristic, void *context,
            size_t *out_path, size_t out_path_capacity,
            size_t *out_path_length);

#endif
