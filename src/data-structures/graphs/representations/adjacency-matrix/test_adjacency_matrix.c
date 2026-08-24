#include "adjacency_matrix.h"
#include "../../graph-view/graph_view.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

struct NeighborLog {
    AdjacencyMatrixNode *visited[8];
    uint64_t weights[8];
    size_t count;
};

struct GraphNeighborLog {
    size_t visited[8];
    uint64_t weights[8];
    size_t count;
};

static bool record_neighbor(AdjacencyMatrixNode *neighbor, uint64_t weight, void *context) {
    struct NeighborLog *log = context;

    log->visited[log->count] = neighbor;
    log->weights[log->count] = weight;
    log->count++;
    return true;
}

static bool record_graph_neighbor(size_t neighbor, uint64_t weight, void *context) {
    struct GraphNeighborLog *log = context;

    log->visited[log->count] = neighbor;
    log->weights[log->count] = weight;
    log->count++;
    return true;
}

static void test_dynamic_nodes_preserve_payloads_and_edges(void) {
    int values[] = { 10, 20, 30 };
    AdjacencyMatrix *graph = adjacency_matrix_create(true);
    AdjacencyMatrixNode *nodes[3] = { NULL };
    AdjacencyMatrixNode *node = NULL;
    void *value = NULL;
    uint64_t weight = 0U;

    assert(graph != NULL);
    assert(adjacency_matrix_node_count(graph) == 0U);
    assert(adjacency_matrix_add_node(graph, &values[0], &nodes[0]));
    assert(adjacency_matrix_add_node(graph, &values[1], &nodes[1]));
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[1], 7U));
    assert(adjacency_matrix_add_node(graph, &values[2], &nodes[2]));
    assert(adjacency_matrix_node_count(graph) == 3U);
    assert(adjacency_matrix_node_at(graph, 0U, &node) && node == nodes[0]);
    assert(adjacency_matrix_node_at(graph, 2U, &node) && node == nodes[2]);
    assert(!adjacency_matrix_node_at(graph, 3U, &node));
    assert(adjacency_matrix_node_value(nodes[1], &value) && value == &values[1]);
    assert(adjacency_matrix_has_edge(graph, nodes[0], nodes[1]));
    assert(adjacency_matrix_get_edge_weight(graph, nodes[0], nodes[1], &weight));
    assert(weight == 7U);
    adjacency_matrix_destroy(graph);
}

static void test_directed_add_remove_and_undirected_symmetry(void) {
    int values[] = { 0, 1, 2 };
    AdjacencyMatrix *directed = adjacency_matrix_create(true);
    AdjacencyMatrix *undirected = adjacency_matrix_create(false);
    AdjacencyMatrixNode *a = NULL;
    AdjacencyMatrixNode *b = NULL;
    uint64_t weight = 0U;

    assert(directed != NULL && undirected != NULL);
    assert(adjacency_matrix_add_node(directed, &values[0], &a));
    assert(adjacency_matrix_add_node(directed, &values[1], &b));
    assert(adjacency_matrix_add_edge(directed, a, b, 7U));
    assert(adjacency_matrix_has_edge(directed, a, b));
    assert(!adjacency_matrix_has_edge(directed, b, a));
    assert(adjacency_matrix_remove_edge(directed, a, b));
    assert(!adjacency_matrix_has_edge(directed, a, b));

    assert(adjacency_matrix_add_node(undirected, &values[0], &a));
    assert(adjacency_matrix_add_node(undirected, &values[2], &b));
    assert(adjacency_matrix_add_edge(undirected, a, b, 9U));
    assert(adjacency_matrix_has_edge(undirected, a, b));
    assert(adjacency_matrix_has_edge(undirected, b, a));
    assert(adjacency_matrix_get_edge_weight(undirected, b, a, &weight));
    assert(weight == 9U);
    assert(adjacency_matrix_remove_edge(undirected, b, a));
    assert(!adjacency_matrix_has_edge(undirected, a, b));
    adjacency_matrix_destroy(directed);
    adjacency_matrix_destroy(undirected);
}

static void test_duplicate_neighbor_and_invalid_node_behavior(void) {
    int values[] = { 0, 1, 2 };
    AdjacencyMatrix *graph = adjacency_matrix_create(true);
    AdjacencyMatrix *other = adjacency_matrix_create(true);
    AdjacencyMatrixNode *nodes[2] = { NULL };
    AdjacencyMatrixNode *foreign = NULL;
    struct NeighborLog log = { .count = 0U };

    assert(graph != NULL && other != NULL);
    assert(adjacency_matrix_add_node(graph, &values[0], &nodes[0]));
    assert(adjacency_matrix_add_node(graph, &values[1], &nodes[1]));
    assert(adjacency_matrix_add_node(other, &values[2], &foreign));
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[1], 4U));
    assert(!adjacency_matrix_add_edge(graph, nodes[0], nodes[1], 4U));
    assert(!adjacency_matrix_remove_edge(graph, nodes[1], nodes[0]));
    assert(adjacency_matrix_neighbors(graph, nodes[0], record_neighbor, &log));
    assert(log.count == 1U && log.visited[0] == nodes[1] && log.weights[0] == 4U);
    assert(!adjacency_matrix_add_edge(graph, nodes[0], foreign, 1U));
    assert(!adjacency_matrix_has_edge(graph, nodes[0], foreign));
    assert(!adjacency_matrix_neighbors(graph, foreign, record_neighbor, &log));
    assert(!adjacency_matrix_add_node(NULL, &values[0], &nodes[0]));
    assert(adjacency_matrix_node_count(NULL) == 0U);
    assert(adjacency_matrix_edge_count(NULL) == 0U);
    adjacency_matrix_destroy(graph);
    adjacency_matrix_destroy(other);
}

static void test_graph_view_maps_node_indexes(void) {
    int values[] = { 0, 1, 2 };
    AdjacencyMatrix *graph = adjacency_matrix_create(true);
    AdjacencyMatrixNode *nodes[3] = { NULL };
    GraphView view = { 0 };
    struct GraphNeighborLog log = { .count = 0U };

    assert(graph != NULL);
    for (size_t index = 0U; index < 3U; index++) {
        assert(adjacency_matrix_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[2], 12U));
    assert(adjacency_matrix_graph_view(graph, &view));
    assert(graph_view_is_valid(&view));
    assert(graph_view_vertex_count(&view) == 3U);
    assert(graph_view_neighbors(&view, 0U, record_graph_neighbor, &log));
    assert(log.count == 1U && log.visited[0] == 2U && log.weights[0] == 12U);
    assert(!adjacency_matrix_graph_view(NULL, &view));
    assert(!adjacency_matrix_graph_view(graph, NULL));
    adjacency_matrix_destroy(graph);
}

int main(void) {
    test_dynamic_nodes_preserve_payloads_and_edges();
    test_directed_add_remove_and_undirected_symmetry();
    test_duplicate_neighbor_and_invalid_node_behavior();
    test_graph_view_maps_node_indexes();
    return 0;
}
