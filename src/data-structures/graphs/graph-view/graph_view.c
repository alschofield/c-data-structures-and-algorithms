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

    // Rejects a view that cannot retrieve Nodes by dense index.
    if (view->node_at == NULL) {
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

// Validates and delegates dense Node lookup to the backing graph.
bool graph_view_node_at(const GraphView *view, size_t index, Node **out_node) {
    // Rejects a missing or incomplete adapter.
    if (!graph_view_is_valid(view)) {
        return false;
    }

    // Rejects a missing caller-owned output location.
    if (out_node == NULL) {
        return false;
    }

    // Rejects an index outside the backing graph's active node range.
    if (index >= graph_view_vertex_count(view)) {
        return false;
    }

    // Lets the concrete adapter return the graph-owned Node handle.
    return view->node_at(view->context, index, out_node);
}

// Validates and delegates weighted neighbor iteration to the backing graph.
bool graph_view_neighbors(const GraphView *view, const Node *node,
                          GraphViewVisitFn visit, void *context) {
    // Rejects a missing or incomplete adapter.
    if (!graph_view_is_valid(view)) {
        return false;
    }

    // Rejects a missing node or visitor callback.
    if (node == NULL) {
        return false;
    }

    if (visit == NULL) {
        return false;
    }

    // Lets the concrete graph enumerate neighbors and propagate visitor stop.
    return view->neighbors(view->context, node, visit, context);
}
