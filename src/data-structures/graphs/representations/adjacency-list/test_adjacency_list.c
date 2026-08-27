#include "adjacency_list.h"
#include "../../graph-view/graph_view.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

struct NeighborLog {
    Node *visited[8];
    uint64_t weights[8];
    size_t count;
};

struct GraphNeighborLog {
    Node *visited[8];
    uint64_t weights[8];
    size_t count;
};

static bool record_neighbor(Node *neighbor, uint64_t weight, void *context) {
    struct NeighborLog *log = context;

    log->visited[log->count] = neighbor;
    log->weights[log->count] = weight;
    log->count++;
    return true;
}

static bool stop_after_first(Node *neighbor, uint64_t weight, void *context) {
    record_neighbor(neighbor, weight, context);
    return false;
}

static bool record_graph_neighbor(Node *neighbor, uint64_t weight, void *context) {
    struct GraphNeighborLog *log = context;

    log->visited[log->count] = neighbor;
    log->weights[log->count] = weight;
    log->count++;
    return true;
}

static void test_dynamic_nodes_preserve_payload_identity(void) {
    int first_value = 10;
    int second_value = 20;
    int third_value = 30;
    AdjacencyList *graph = adjacency_list_create(true);
    Node *first = NULL;
    Node *second = NULL;
    Node *third = NULL;
    Node *node = NULL;
    void *value = NULL;

    assert(graph != NULL);
    assert(adjacency_list_node_count(graph) == 0U);
    assert(adjacency_list_add_node(graph, &first_value, &first));
    assert(adjacency_list_add_node(graph, &second_value, &second));
    assert(adjacency_list_add_node(graph, &third_value, &third));
    assert(adjacency_list_node_count(graph) == 3U);
    assert(adjacency_list_node_at(graph, 0U, &node) && node == first);
    assert(adjacency_list_node_at(graph, 1U, &node) && node == second);
    assert(adjacency_list_node_at(graph, 2U, &node) && node == third);
    assert(!adjacency_list_node_at(graph, 3U, &node));
    assert(adjacency_list_node_value(second, &value) && value == &second_value);
    assert(adjacency_list_node_value(third, &value) && value == &third_value);
    adjacency_list_destroy(graph);
}

static void test_directed_and_undirected_edges(void) {
    int values[] = { 0, 1, 2 };
    AdjacencyList *directed = adjacency_list_create(true);
    AdjacencyList *undirected = adjacency_list_create(false);
    Node *a = NULL;
    Node *b = NULL;
    Node *c = NULL;

    assert(directed != NULL && undirected != NULL);
    assert(adjacency_list_add_node(directed, &values[0], &a));
    assert(adjacency_list_add_node(directed, &values[1], &b));
    assert(adjacency_list_add_node(directed, &values[2], &c));
    assert(adjacency_list_add_edge(directed, a, b, 7U));
    assert(adjacency_list_add_edge(directed, a, c, 3U));
    assert(adjacency_list_add_edge(directed, c, a, 1U));
    assert(adjacency_list_edge_count(directed) == 3U);
    assert(adjacency_list_has_edge(directed, a, b));
    assert(!adjacency_list_has_edge(directed, b, a));
    assert(adjacency_list_has_edge(directed, c, a));

    assert(adjacency_list_add_node(undirected, &values[0], &a));
    assert(adjacency_list_add_node(undirected, &values[1], &b));
    assert(adjacency_list_add_edge(undirected, a, b, 9U));
    assert(adjacency_list_has_edge(undirected, a, b));
    assert(adjacency_list_has_edge(undirected, b, a));
    adjacency_list_destroy(directed);
    adjacency_list_destroy(undirected);
}

static void test_duplicates_and_self_loops_preserve_logical_counts(void) {
    int values[] = { 0, 1 };
    AdjacencyList *directed = adjacency_list_create(true);
    AdjacencyList *undirected = adjacency_list_create(false);
    Node *a = NULL;
    Node *b = NULL;
    struct NeighborLog log = { .count = 0U };

    assert(directed != NULL && undirected != NULL);
    assert(adjacency_list_add_node(directed, &values[0], &a));
    assert(adjacency_list_add_node(directed, &values[1], &b));
    assert(adjacency_list_add_edge(directed, a, b, 2U));
    assert(adjacency_list_add_edge(directed, a, b, 7U));
    assert(adjacency_list_add_edge(directed, a, a, 3U));
    assert(adjacency_list_add_edge(directed, a, a, 5U));
    assert(adjacency_list_edge_count(directed) == 2U);
    assert(adjacency_list_neighbors(directed, a, record_neighbor, &log));
    assert(log.count == 2U);
    assert(log.visited[0] == b && log.weights[0] == 7U);
    assert(log.visited[1] == a && log.weights[1] == 5U);

    log.count = 0U;
    assert(adjacency_list_add_node(undirected, &values[0], &a));
    assert(adjacency_list_add_node(undirected, &values[1], &b));
    assert(adjacency_list_add_edge(undirected, a, b, 4U));
    assert(adjacency_list_add_edge(undirected, b, a, 9U));
    assert(adjacency_list_edge_count(undirected) == 1U);
    assert(adjacency_list_neighbors(undirected, a, record_neighbor, &log));
    assert(log.count == 1U && log.visited[0] == b && log.weights[0] == 9U);

    log.count = 0U;
    assert(adjacency_list_neighbors(undirected, b, record_neighbor, &log));
    assert(log.count == 1U && log.visited[0] == a && log.weights[0] == 9U);
    adjacency_list_destroy(directed);
    adjacency_list_destroy(undirected);
}

static void test_neighbor_iteration(void) {
    int values[] = { 0, 1, 2, 3 };
    AdjacencyList *graph = adjacency_list_create(true);
    Node *nodes[4] = { NULL };
    struct NeighborLog log = { .count = 0U };

    assert(graph != NULL);
    for (size_t index = 0U; index < 4U; index++) {
        assert(adjacency_list_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_list_add_edge(graph, nodes[1], nodes[0], 5U));
    assert(adjacency_list_add_edge(graph, nodes[1], nodes[2], 8U));
    assert(adjacency_list_add_edge(graph, nodes[1], nodes[3], 2U));
    assert(adjacency_list_neighbors(graph, nodes[1], record_neighbor, &log));
    assert(log.count == 3U);
    assert(log.visited[0] == nodes[0] && log.weights[0] == 5U);
    assert(log.visited[1] == nodes[2] && log.weights[1] == 8U);
    assert(log.visited[2] == nodes[3] && log.weights[2] == 2U);

    log.count = 0U;
    assert(!adjacency_list_neighbors(graph, nodes[1], stop_after_first, &log));
    assert(log.count == 1U);
    log.count = 0U;
    assert(adjacency_list_neighbors(graph, nodes[0], record_neighbor, &log));
    assert(log.count == 0U);
    adjacency_list_destroy(graph);
}

static void test_foreign_and_null_nodes_are_rejected(void) {
    int first_value = 1;
    int second_value = 2;
    AdjacencyList *graph = adjacency_list_create(true);
    AdjacencyList *other = adjacency_list_create(true);
    Node *node = NULL;
    Node *foreign = NULL;
    struct NeighborLog log = { .count = 0U };

    assert(graph != NULL && other != NULL);
    assert(adjacency_list_add_node(graph, &first_value, &node));
    assert(adjacency_list_add_node(other, &second_value, &foreign));
    assert(!adjacency_list_add_edge(graph, node, foreign, 1U));
    assert(!adjacency_list_add_edge(graph, NULL, node, 1U));
    assert(!adjacency_list_has_edge(graph, node, foreign));
    assert(!adjacency_list_neighbors(graph, foreign, record_neighbor, &log));
    assert(!adjacency_list_add_node(NULL, &first_value, &node));
    assert(adjacency_list_node_count(NULL) == 0U);
    assert(adjacency_list_edge_count(NULL) == 0U);
    adjacency_list_destroy(graph);
    adjacency_list_destroy(other);
}

static void test_graph_view_maps_node_indexes(void) {
    int values[] = { 0, 1, 2 };
    AdjacencyList *graph = adjacency_list_create(true);
    Node *nodes[3] = { NULL };
    GraphView view = { 0 };
    struct GraphNeighborLog log = { .count = 0U };
    Node *node = NULL;

    assert(graph != NULL);
    for (size_t index = 0U; index < 3U; index++) {
        assert(adjacency_list_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[2], 12U));
    assert(adjacency_list_graph_view(graph, &view));
    assert(graph_view_is_valid(&view));
    assert(graph_view_vertex_count(&view) == 3U);
    assert(graph_view_node_at(&view, 1U, &node) && node == nodes[1]);
    assert(!graph_view_node_at(&view, 3U, &node));
    assert(graph_view_neighbors(&view, nodes[0], record_graph_neighbor, &log));
    assert(log.count == 1U && log.visited[0] == nodes[2] && log.weights[0] == 12U);
    assert(!adjacency_list_graph_view(NULL, &view));
    assert(!adjacency_list_graph_view(graph, NULL));
    adjacency_list_destroy(graph);
}

int main(void) {
    test_dynamic_nodes_preserve_payload_identity();
    test_directed_and_undirected_edges();
    test_duplicates_and_self_loops_preserve_logical_counts();
    test_neighbor_iteration();
    test_foreign_and_null_nodes_are_rejected();
    test_graph_view_maps_node_indexes();
    return 0;
}
