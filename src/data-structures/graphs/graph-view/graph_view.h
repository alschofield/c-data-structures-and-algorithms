// Declares size_t.
#include <stddef.h>
// Declares bool.
#include <stdbool.h>
// Declares uint64_t.
#include <stdint.h>

// Names one representation-independent read-only graph adapter.
typedef struct GraphView GraphView;
// Visits one weighted neighbor and returns false to stop iteration.
typedef bool (*GraphViewVisitFn)(size_t neighbor, uint64_t weight, void *context);
// Reports the dense internal node count of a backing graph.
typedef size_t (*GraphViewVertexCountFn)(const void *graph_context);
// Iterates one backing graph node's weighted outgoing neighbors.
typedef bool (*GraphViewNeighborsFn)(const void *graph_context, size_t vertex, GraphViewVisitFn visit, void *context);

// Stores non-owning callbacks that adapt a concrete graph to generic algorithms.
struct GraphView {
    // Points to the backing graph without taking ownership of it.
    const void *context;
    // Reports the backing graph's dense node count.
    GraphViewVertexCountFn vertex_count;
    // Iterates weighted neighbors through the backing graph representation.
    GraphViewNeighborsFn neighbors;
};

// Checks that every field required to use a GraphView is present.
bool graph_view_is_valid(const GraphView *view);
// Returns the backing graph's node count or zero for an invalid view.
size_t graph_view_vertex_count(const GraphView *view);
// Validates and delegates weighted neighbor iteration to the backing graph.
bool graph_view_neighbors(const GraphView *view, size_t vertex, GraphViewVisitFn visit, void *context);
