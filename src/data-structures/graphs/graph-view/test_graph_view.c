#include "graph_view.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

struct TestEdge {
    Node *neighbor;
    uint64_t weight;
};

struct TestGraph {
    Node **nodes;
    const struct TestEdge *edges;
    size_t edge_count;
    size_t vertex_count;
};

struct VisitLog {
    Node *neighbors[4];
    uint64_t weights[4];
    size_t count;
};

static size_t test_vertex_count(const void *context) {
    const struct TestGraph *graph = context;

    return graph->vertex_count;
}

static bool test_node_at(const void *context, size_t index, Node **out_node) {
    const struct TestGraph *graph = context;

    if (index >= graph->vertex_count || out_node == NULL) {
        return false;
    }

    *out_node = graph->nodes[index];
    return true;
}

static bool test_neighbors(const void *context, const Node *node,
                           GraphViewVisitFn visit, void *visit_context) {
    const struct TestGraph *graph = context;

    if (node->index >= graph->vertex_count) {
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

static bool record_edge(Node *neighbor, uint64_t weight, void *context) {
    struct VisitLog *log = context;

    log->neighbors[log->count] = neighbor;
    log->weights[log->count] = weight;
    log->count++;
    return true;
}

static bool stop_after_first(Node *neighbor, uint64_t weight, void *context) {
    struct VisitLog *log = context;

    log->neighbors[log->count] = neighbor;
    log->weights[log->count] = weight;
    log->count++;
    return false;
}

static void test_valid_custom_adapter(void) {
    static Node nodes[] = { { .index = 0U }, { .index = 1U }, { .index = 2U } };
    static Node *node_pointers[] = { &nodes[0], &nodes[1], &nodes[2] };
    static const struct TestEdge edges[] = { { .neighbor = &nodes[1], .weight = 4U },
                                             { .neighbor = &nodes[2], .weight = 1U } };
    struct TestGraph graph = { .nodes = node_pointers, .edges = edges, .edge_count = 2U,
                               .vertex_count = 3U };
    GraphView view = { .context = &graph, .vertex_count = test_vertex_count,
                       .node_at = test_node_at, .neighbors = test_neighbors };
    struct VisitLog log = { .count = 0U };
    Node *node = NULL;

    assert(graph_view_is_valid(&view));
    assert(graph_view_vertex_count(&view) == 3U);
    assert(graph_view_node_at(&view, 0U, &node) && node == &nodes[0]);
    assert(graph_view_node_at(&view, 2U, &node) && node == &nodes[2]);
    assert(!graph_view_node_at(&view, 3U, &node));
    assert(graph_view_neighbors(&view, &nodes[0], record_edge, &log));
    assert(log.count == 2U);
    assert(log.neighbors[0] == &nodes[1]);
    assert(log.weights[0] == 4U);
    assert(log.neighbors[1] == &nodes[2]);
    assert(log.weights[1] == 1U);
}

static void test_invalid_views(void) {
    GraphView missing_context = { .context = NULL, .vertex_count = test_vertex_count,
                                   .node_at = test_node_at, .neighbors = test_neighbors };
    GraphView missing_vertex_count = { .context = &missing_context,
                                        .vertex_count = NULL, .node_at = test_node_at,
                                        .neighbors = test_neighbors };
    GraphView missing_node_at = { .context = &missing_context,
                                  .vertex_count = test_vertex_count, .node_at = NULL,
                                  .neighbors = test_neighbors };
    GraphView missing_neighbors = { .context = &missing_context,
                                     .vertex_count = test_vertex_count, .node_at = test_node_at,
                                     .neighbors = NULL };

    assert(!graph_view_is_valid(NULL));
    assert(!graph_view_is_valid(&missing_context));
    assert(!graph_view_is_valid(&missing_vertex_count));
    assert(!graph_view_is_valid(&missing_node_at));
    assert(!graph_view_is_valid(&missing_neighbors));
    assert(graph_view_vertex_count(NULL) == 0U);
    assert(!graph_view_neighbors(NULL, NULL, record_edge, NULL));
}

static void test_visitor_can_stop_iteration(void) {
    static Node nodes[] = { { .index = 0U }, { .index = 1U }, { .index = 2U } };
    static Node *node_pointers[] = { &nodes[0], &nodes[1], &nodes[2] };
    static const struct TestEdge edges[] = { { .neighbor = &nodes[1], .weight = 1U },
                                             { .neighbor = &nodes[2], .weight = 1U } };
    struct TestGraph graph = { .nodes = node_pointers, .edges = edges, .edge_count = 2U,
                               .vertex_count = 3U };
    GraphView view = { .context = &graph, .vertex_count = test_vertex_count,
                       .node_at = test_node_at, .neighbors = test_neighbors };
    struct VisitLog log = { .count = 0U };

    assert(!graph_view_neighbors(&view, &nodes[0], stop_after_first, &log));
    assert(log.count == 1U);
}

int main(void) {
    test_valid_custom_adapter();
    test_invalid_views();
    test_visitor_can_stop_iteration();
    return 0;
}
