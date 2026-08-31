#ifndef KRUSKAL_H
#define KRUSKAL_H

// Declares GraphView, Node, uint64_t, and size_t.
#include "../../../data-structures/graphs/graph-view/graph_view.h"

// Stores one selected weighted edge in a minimum spanning forest.
typedef struct KruskalEdge {
    // Identifies one graph-owned endpoint.
    Node *from;
    // Identifies the other graph-owned endpoint.
    Node *to;
    // Stores the nonnegative cost of connecting the endpoints.
    uint64_t weight;
} KruskalEdge;

typedef struct KruskalEdgeFinderContext KruskalEdgeFinderContext;

// Builds a minimum spanning forest for an undirected weighted GraphView.
bool kruskal_minimum_spanning_forest(const GraphView *graph, KruskalEdge *out_edges, size_t out_edge_capacity, size_t *out_edge_count, uint64_t *out_total_weight);

#endif
