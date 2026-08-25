#include "graph_view.h"

// Checks that a view has backing context and both required callbacks.
bool graph_view_is_valid(const GraphView *view) {
    // Rejects a missing view object.
    if (view == NULL) {
        return false;
    }

    // Rejects a view with no backing graph context.
    if (view->context == NULL) {
        return false;
    }

    // Rejects a view that cannot report its node count.
    if (view->vertex_count == NULL) {
        return false;
    }

    // Rejects a view that cannot enumerate neighbors.
    if (view->neighbors == NULL) {
        return false;
    }

    // Reports a fully usable adapter.
    return true;
}

// Returns the backing graph's dense node count.
size_t graph_view_vertex_count(const GraphView *view) {
    // Treats an invalid view as having zero addressable nodes.
    if (!graph_view_is_valid(view)) {
        return 0U;
    }

    // Delegates the count query to the concrete graph adapter.
    return view->vertex_count(view->context);
}

// Validates and delegates weighted neighbor iteration to the backing graph.
bool graph_view_neighbors(const GraphView *view, size_t vertex, GraphViewVisitFn visit, void *context) {
    // Rejects a missing or incomplete adapter.
    if (!graph_view_is_valid(view)) {
        return false;
    }

    // Rejects a missing visitor callback.
    if (visit == NULL) {
        return false;
    }

    // Rejects an index outside the backing graph's dense node range.
    if (vertex >= graph_view_vertex_count(view)) {
        return false;
    }

    // Lets the concrete graph enumerate neighbors and propagate visitor stop.
    return view->neighbors(view->context, vertex, visit, context);
}
