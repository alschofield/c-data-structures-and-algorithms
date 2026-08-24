#include "graph_view.h"


struct GraphView {
    const void *context;
    GraphViewVertexCountFn vertex_count;
    GraphViewNeighborsFn neighbors;
};

bool graph_view_is_valid(const GraphView *view) {
    if (view == NULL) {
        return false;
    }

    return false;
}

size_t graph_view_vertex_count(const GraphView *view) {
    if (view == NULL) {
        return 0U;
    }

    return 0U;
}

bool graph_view_neighbors(const GraphView *view, size_t vertex, GraphViewVisitFn visit, void *context) {
    if (view == NULL) {
        return false;
    }

    if (vertex == NULL) {
        return false;
    }

    if (visit == NULL) {
        return false;
    }

    return false;
}
