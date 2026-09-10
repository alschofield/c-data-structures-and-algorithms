#ifndef GRAPH_VIEW_H
#define GRAPH_VIEW_H

// Declares size_t.
#include <stddef.h>
// Declares bool.
#include <stdbool.h>
// Declares uint64_t.
#include <stdint.h>

// Names one representation-independent read-only graph adapter.
typedef struct GraphView GraphView;
// Visits one weighted neighbor index and returns false to stop iteration.
typedef bool (*GraphViewVisitFn)(size_t neighbor_index, uint64_t weight,
                                 void *context);
// Reports the node count of a backing graph.
typedef size_t (*GraphViewVertexCountFn)(const void *graph_context);
// Reports the directed status of a backing graph.
typedef bool (*GraphViewIsDirectedFn)(const void *graph_context);
// Validates lookup of one backing graph node by dense internal index.
typedef bool (*GraphViewNodeAtFn)(const void *graph_context, size_t index);
// Iterates one backing graph index's weighted outgoing neighbors.
typedef bool (*GraphViewNeighborsFn)(const void *graph_context, size_t index,
                                      GraphViewVisitFn visit, void *context);

// Stores non-owning callbacks that adapt a concrete graph to generic algorithms.
struct GraphView {
    // Points to the backing graph without taking ownership of it.
    const void *context;
    // Reports the backing graph's dense node count.
    GraphViewVertexCountFn vertex_count;
    // Validates lookup of one backing graph node by dense internal index.
    GraphViewNodeAtFn node_at;
    // Iterates weighted neighbors through the backing graph representation.
    GraphViewNeighborsFn neighbors;
    // Reports the directed status of a backing graph.
    GraphViewIsDirectedFn is_directed;
};

// Checks that every field required to use a GraphView is present.
bool graph_view_is_valid(const GraphView *view);
// Returns the backing graph's node count or zero for an invalid view.
size_t graph_view_vertex_count(const GraphView *view);
// Validates lookup of one backing graph node through its concrete adapter.
bool graph_view_node_at(const GraphView *view, size_t index);
// Validates and delegates weighted neighbor iteration to the backing graph.
bool graph_view_neighbors(const GraphView *view, size_t index,
                           GraphViewVisitFn visit, void *context);
// Reports the directed status of a backing graph.
bool graph_view_is_directed(const GraphView *view);

#endif
