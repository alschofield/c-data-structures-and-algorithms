#include "breadth_first_search.h"
#include "../../../data-structures/graphs/representations/adjacency-list/adjacency_list.h"
#include "../../../data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

#include <assert.h>
#include <stddef.h>

struct VisitLog {
    Node *order[4];
    size_t count;
};

static bool record_visit(Node *node, void *context) {
    struct VisitLog *log = context;

    log->order[log->count] = node;
    log->count++;
    return true;
}

static bool stop_after_first(Node *node, void *context) {
    record_visit(node, context);
    return false;
}

static void assert_weight_agnostic_traversal(const GraphView *view, Node *nodes[4]) {
    struct VisitLog log = { .count = 0U };

    assert(breadth_first_search(view, nodes[0], record_visit, &log));
    assert(log.count == 3U);
    assert(log.order[0] == nodes[0]);
    assert(log.order[1] == nodes[1]);
    assert(log.order[2] == nodes[2]);

    log.count = 0U;
    assert(!breadth_first_search(view, nodes[0], stop_after_first, &log));
    assert(log.count == 1U && log.order[0] == nodes[0]);
    assert(!breadth_first_search(view, NULL, record_visit, &log));
    assert(!breadth_first_search(view, nodes[0], NULL, &log));
}

static void test_adjacency_list_adapter(void) {
    int values[] = { 0, 1, 2, 3 };
    AdjacencyList *graph = adjacency_list_create(true);
    Node *nodes[4] = { NULL };
    GraphView view = { 0 };

    assert(graph != NULL);
    for (size_t index = 0U; index < 4U; index++) {
        assert(adjacency_list_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[1], 99U));
    assert(adjacency_list_add_edge(graph, nodes[1], nodes[2], 1U));
    assert(adjacency_list_graph_view(graph, &view));
    assert_weight_agnostic_traversal(&view, nodes);
    adjacency_list_destroy(graph);
}

static void test_adjacency_matrix_adapter(void) {
    int values[] = { 0, 1, 2, 3 };
    AdjacencyMatrix *graph = adjacency_matrix_create(true);
    Node *nodes[4] = { NULL };
    GraphView view = { 0 };

    assert(graph != NULL);
    for (size_t index = 0U; index < 4U; index++) {
        assert(adjacency_matrix_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[1], 99U));
    assert(adjacency_matrix_add_edge(graph, nodes[1], nodes[2], 1U));
    assert(adjacency_matrix_graph_view(graph, &view));
    assert_weight_agnostic_traversal(&view, nodes);
    adjacency_matrix_destroy(graph);
}

int main(void) {
    test_adjacency_list_adapter();
    test_adjacency_matrix_adapter();
    return 0;
}
