#include "graph_view.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

struct TestEdge {
    size_t neighbor;
    uint64_t weight;
};

struct TestGraph {
    const struct TestEdge *edges;
    size_t edge_count;
    size_t vertex_count;
};

struct VisitLog {
    size_t neighbors[4];
    uint64_t weights[4];
    size_t count;
};

static size_t test_vertex_count(const void *context) {
    const struct TestGraph *graph = context;

    return graph->vertex_count;
}

static bool test_neighbors(const void *context, size_t vertex,
                           GraphViewVisitFn visit, void *visit_context) {
    const struct TestGraph *graph = context;

    if (vertex >= graph->vertex_count) {
        return false;
    }
    for (size_t index = 0U; index < graph->edge_count; ++index) {
        if (!visit(graph->edges[index].neighbor, graph->edges[index].weight,
                   visit_context)) {
            return false;
        }
    }
    return true;
}

static bool record_edge(size_t neighbor, uint64_t weight, void *context) {
    struct VisitLog *log = context;

    log->neighbors[log->count] = neighbor;
    log->weights[log->count] = weight;
    log->count++;
    return true;
}

static bool stop_after_first(size_t neighbor, uint64_t weight, void *context) {
    struct VisitLog *log = context;

    log->neighbors[log->count] = neighbor;
    log->weights[log->count] = weight;
    log->count++;
    return false;
}

static void test_valid_custom_adapter(void) {
    static const struct TestEdge edges[] = { { .neighbor = 1U, .weight = 4U },
                                             { .neighbor = 2U, .weight = 1U } };
    struct TestGraph graph = { .edges = edges, .edge_count = 2U, .vertex_count = 3U };
    GraphView view = { .context = &graph, .vertex_count = test_vertex_count,
                       .neighbors = test_neighbors };
    struct VisitLog log = { .count = 0U };

    assert(graph_view_is_valid(&view));
    assert(graph_view_vertex_count(&view) == 3U);
    assert(graph_view_neighbors(&view, 0U, record_edge, &log));
    assert(log.count == 2U);
    assert(log.neighbors[0] == 1U);
    assert(log.weights[0] == 4U);
    assert(log.neighbors[1] == 2U);
    assert(log.weights[1] == 1U);
}

static void test_invalid_views(void) {
    GraphView missing_context = { .context = NULL, .vertex_count = test_vertex_count,
                                  .neighbors = test_neighbors };
    GraphView missing_vertex_count = { .context = &missing_context,
                                       .vertex_count = NULL, .neighbors = test_neighbors };
    GraphView missing_neighbors = { .context = &missing_context,
                                    .vertex_count = test_vertex_count, .neighbors = NULL };

    assert(!graph_view_is_valid(NULL));
    assert(!graph_view_is_valid(&missing_context));
    assert(!graph_view_is_valid(&missing_vertex_count));
    assert(!graph_view_is_valid(&missing_neighbors));
    assert(graph_view_vertex_count(NULL) == 0U);
    assert(!graph_view_neighbors(NULL, 0U, record_edge, NULL));
}

static void test_visitor_can_stop_iteration(void) {
    static const struct TestEdge edges[] = { { .neighbor = 1U, .weight = 1U },
                                             { .neighbor = 2U, .weight = 1U } };
    struct TestGraph graph = { .edges = edges, .edge_count = 2U, .vertex_count = 3U };
    GraphView view = { .context = &graph, .vertex_count = test_vertex_count,
                       .neighbors = test_neighbors };
    struct VisitLog log = { .count = 0U };

    assert(!graph_view_neighbors(&view, 0U, stop_after_first, &log));
    assert(log.count == 1U);
}

int main(void) {
    test_valid_custom_adapter();
    test_invalid_views();
    test_visitor_can_stop_iteration();
    return 0;
}
