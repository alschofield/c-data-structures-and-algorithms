#include "graph_view.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

struct TestEdge {
    size_t neighbor_index;
    uint64_t weight;
};

struct TestGraph {
    const struct TestEdge *edges;
    size_t edge_count;
    size_t vertex_count;
    bool directed;
};

struct VisitLog {
    size_t neighbors[4];
    uint64_t weights[4];
    size_t count;
};

static size_t test_vertex_count(const void *context) {
    return ((const struct TestGraph *)context)->vertex_count;
}

static bool test_is_directed(const void *context) {
    return ((const struct TestGraph *)context)->directed;
}

static bool test_node_at(const void *context, size_t index) {
    return index < ((const struct TestGraph *)context)->vertex_count;
}

static bool test_neighbors(const void *context, size_t source_index,
                           GraphViewVisitFn visit, void *visit_context) {
    const struct TestGraph *graph = context;
    if (source_index >= graph->vertex_count || visit == NULL) return false;
    if (source_index != 0U) return true;

    for (size_t index = 0U; index < graph->edge_count; index++) {
        if (!visit(graph->edges[index].neighbor_index, graph->edges[index].weight,
                   visit_context)) return false;
    }
    return true;
}

static bool record_edge(size_t neighbor_index, uint64_t weight, void *context) {
    struct VisitLog *log = context;

    log->neighbors[log->count] = neighbor_index;
    log->weights[log->count] = weight;
    log->count++;
    return true;
}

static bool stop_after_first(size_t neighbor_index, uint64_t weight, void *context) {
    record_edge(neighbor_index, weight, context);
    return false;
}

static GraphView test_view(struct TestGraph *graph) {
    return (GraphView) {
        .context = graph,
        .vertex_count = test_vertex_count,
        .is_directed = test_is_directed,
        .node_at = test_node_at,
        .neighbors = test_neighbors
    };
}

static void test_valid_custom_adapter(void) {
    static const struct TestEdge edges[] = {
        { .neighbor_index = 1U, .weight = 4U },
        { .neighbor_index = 2U, .weight = 1U }
    };
    struct TestGraph graph = {
        .edges = edges,
        .edge_count = 2U,
        .vertex_count = 3U,
        .directed = true
    };
    GraphView view = test_view(&graph);
    struct VisitLog log = { 0 };

    assert(graph_view_is_valid(&view));
    assert(graph_view_vertex_count(&view) == 3U);
    assert(graph_view_is_directed(&view));
    assert(graph_view_node_at(&view, 0U));
    assert(graph_view_node_at(&view, 2U));
    assert(!graph_view_node_at(&view, 3U));
    assert(graph_view_neighbors(&view, 0U, record_edge, &log));
    assert(log.count == 2U);
    assert(log.neighbors[0] == 1U && log.weights[0] == 4U);
    assert(log.neighbors[1] == 2U && log.weights[1] == 1U);
}

static void test_invalid_views(void) {
    struct TestGraph graph = { .vertex_count = 1U };
    GraphView missing_context = test_view(&graph);
    GraphView missing_vertex_count = test_view(&graph);
    GraphView missing_node_at = test_view(&graph);
    GraphView missing_neighbors = test_view(&graph);
    GraphView missing_directed = test_view(&graph);
    missing_context.context = NULL;
    missing_vertex_count.vertex_count = NULL;
    missing_node_at.node_at = NULL;
    missing_neighbors.neighbors = NULL;
    missing_directed.is_directed = NULL;

    assert(!graph_view_is_valid(NULL));
    assert(!graph_view_is_valid(&missing_context));
    assert(!graph_view_is_valid(&missing_vertex_count));
    assert(!graph_view_is_valid(&missing_node_at));
    assert(!graph_view_is_valid(&missing_neighbors));
    assert(!graph_view_is_valid(&missing_directed));
    assert(graph_view_vertex_count(NULL) == 0U);
    assert(!graph_view_neighbors(NULL, 0U, record_edge, NULL));
}

static void test_visitor_can_stop_iteration(void) {
    static const struct TestEdge edges[] = {
        { .neighbor_index = 1U, .weight = 1U },
        { .neighbor_index = 2U, .weight = 1U }
    };
    struct TestGraph graph = {
        .edges = edges,
        .edge_count = 2U,
        .vertex_count = 3U,
        .directed = false
    };
    GraphView view = test_view(&graph);
    struct VisitLog log = { 0 };

    assert(!graph_view_neighbors(&view, 0U, stop_after_first, &log));
    assert(log.count == 1U);
}

int main(void) {
    test_valid_custom_adapter();
    test_invalid_views();
    test_visitor_can_stop_iteration();
    return 0;
}
