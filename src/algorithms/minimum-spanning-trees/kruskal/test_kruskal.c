// Declares the public Kruskal API under test.
#include "kruskal.h"
// Declares the GraphView contract used by both graph representations.
#include "../../../data-structures/graphs/graph-view/graph_view.h"
// Declares adjacency-list graph construction helpers.
#include "../../../data-structures/graphs/representations/adjacency-list/adjacency_list.h"
// Declares adjacency-matrix graph construction helpers.
#include "../../../data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

// Declares assert.
#include <assert.h>
// Declares size_t.
#include <stddef.h>
// Declares uint64_t and UINT64_MAX.
#include <stdint.h>

// Verifies the selected edges form the expected minimum spanning tree.
static void assert_minimum_spanning_tree(const GraphView *view) {
    KruskalEdge edges[3] = { 0 };
    KruskalEdge too_small_edges[2] = {
        { .weight = 99U },
        { .weight = 99U }
    };
    size_t edge_count = 99U;
    uint64_t total_weight = UINT64_MAX;
    size_t too_small_count = 99U;
    uint64_t too_small_weight = UINT64_MAX;

    assert(kruskal_minimum_spanning_forest(
        view, edges, 3U, &edge_count, &total_weight
    ));
    assert(edge_count == 3U);
    assert(total_weight == 6U);
    assert(edges[0].weight == 1U);
    assert(edges[1].weight == 2U);
    assert(edges[2].weight == 3U);

    assert(!kruskal_minimum_spanning_forest(
        view, too_small_edges, 2U, &too_small_count, &too_small_weight
    ));
    assert(too_small_count == 99U);
    assert(too_small_weight == UINT64_MAX);
    assert(too_small_edges[0].weight == 99U && too_small_edges[1].weight == 99U);
}

// Builds the weighted tree fixture through the adjacency-list adapter.
static void test_adjacency_list_adapter(void) {
    int values[] = { 0, 1, 2, 3 };
    AdjacencyList *graph = adjacency_list_create(false);
    Node *nodes[4] = { NULL };
    GraphView view = { 0 };

    assert(graph != NULL);
    for (size_t index = 0U; index < 4U; index++) {
        assert(adjacency_list_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[1], 1U));
    assert(adjacency_list_add_edge(graph, nodes[1], nodes[2], 2U));
    assert(adjacency_list_add_edge(graph, nodes[2], nodes[3], 3U));
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[2], 4U));
    assert(adjacency_list_add_edge(graph, nodes[1], nodes[3], 5U));
    assert(adjacency_list_graph_view(graph, &view));

    assert_minimum_spanning_tree(&view);
    adjacency_list_destroy(graph);
}

// Builds the same weighted tree fixture through the adjacency-matrix adapter.
static void test_adjacency_matrix_adapter(void) {
    int values[] = { 0, 1, 2, 3 };
    AdjacencyMatrix *graph = adjacency_matrix_create(false);
    Node *nodes[4] = { NULL };
    GraphView view = { 0 };

    assert(graph != NULL);
    for (size_t index = 0U; index < 4U; index++) {
        assert(adjacency_matrix_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[1], 1U));
    assert(adjacency_matrix_add_edge(graph, nodes[1], nodes[2], 2U));
    assert(adjacency_matrix_add_edge(graph, nodes[2], nodes[3], 3U));
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[2], 4U));
    assert(adjacency_matrix_add_edge(graph, nodes[1], nodes[3], 5U));
    assert(adjacency_matrix_graph_view(graph, &view));

    assert_minimum_spanning_tree(&view);
    adjacency_matrix_destroy(graph);
}

// Verifies zero-edge empty and single-Node forests succeed without edge storage.
static void test_empty_and_single_node_forests(void) {
    int value = 0;
    AdjacencyList *empty_graph = adjacency_list_create(false);
    AdjacencyList *single_graph = adjacency_list_create(false);
    Node *single_node = NULL;
    GraphView empty_view = { 0 };
    GraphView single_view = { 0 };
    size_t edge_count = 99U;
    uint64_t total_weight = UINT64_MAX;

    assert(empty_graph != NULL && single_graph != NULL);
    assert(adjacency_list_graph_view(empty_graph, &empty_view));
    assert(kruskal_minimum_spanning_forest(
        &empty_view, NULL, 0U, &edge_count, &total_weight
    ));
    assert(edge_count == 0U && total_weight == 0U);

    assert(adjacency_list_add_node(single_graph, &value, &single_node));
    assert(adjacency_list_graph_view(single_graph, &single_view));
    edge_count = 99U;
    total_weight = UINT64_MAX;
    assert(kruskal_minimum_spanning_forest(
        &single_view, NULL, 0U, &edge_count, &total_weight
    ));
    assert(edge_count == 0U && total_weight == 0U);

    adjacency_list_destroy(empty_graph);
    adjacency_list_destroy(single_graph);
}

// Verifies disconnected components return a minimum spanning forest.
static void test_disconnected_forest(void) {
    int values[] = { 0, 1, 2, 3, 4 };
    AdjacencyList *graph = adjacency_list_create(false);
    Node *nodes[5] = { NULL };
    GraphView view = { 0 };
    KruskalEdge edges[2] = { 0 };
    size_t edge_count = 0U;
    uint64_t total_weight = 0U;

    assert(graph != NULL);
    for (size_t index = 0U; index < 5U; index++) {
        assert(adjacency_list_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[1], 2U));
    assert(adjacency_list_add_edge(graph, nodes[2], nodes[3], 1U));
    assert(adjacency_list_graph_view(graph, &view));

    assert(kruskal_minimum_spanning_forest(
        &view, edges, 2U, &edge_count, &total_weight
    ));
    assert(edge_count == 2U && total_weight == 3U);
    assert(edges[0].weight == 1U && edges[1].weight == 2U);
    adjacency_list_destroy(graph);
}

// Verifies directed GraphViews are rejected before any forest is selected.
static void test_directed_graph_rejected(void) {
    int values[] = { 0, 1 };
    AdjacencyList *graph = adjacency_list_create(true);
    Node *nodes[2] = { NULL };
    GraphView view = { 0 };
    KruskalEdge edge = { .weight = 99U };
    size_t edge_count = 99U;
    uint64_t total_weight = UINT64_MAX;

    assert(graph != NULL);
    assert(adjacency_list_add_node(graph, &values[0], &nodes[0]));
    assert(adjacency_list_add_node(graph, &values[1], &nodes[1]));
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[1], 1U));
    assert(adjacency_list_graph_view(graph, &view));

    assert(!kruskal_minimum_spanning_forest(
        &view, &edge, 1U, &edge_count, &total_weight
    ));
    assert(edge_count == 99U && total_weight == UINT64_MAX && edge.weight == 99U);
    adjacency_list_destroy(graph);
}

// Verifies total-weight overflow fails without modifying caller output.
static void test_total_weight_overflow(void) {
    int values[] = { 0, 1, 2 };
    AdjacencyList *graph = adjacency_list_create(false);
    Node *nodes[3] = { NULL };
    GraphView view = { 0 };
    KruskalEdge edges[2] = {
        { .weight = 99U },
        { .weight = 99U }
    };
    size_t edge_count = 99U;
    uint64_t total_weight = UINT64_MAX;

    assert(graph != NULL);
    for (size_t index = 0U; index < 3U; index++) {
        assert(adjacency_list_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[1], UINT64_MAX));
    assert(adjacency_list_add_edge(graph, nodes[1], nodes[2], 1U));
    assert(adjacency_list_graph_view(graph, &view));

    assert(!kruskal_minimum_spanning_forest(
        &view, edges, 2U, &edge_count, &total_weight
    ));
    assert(edge_count == 99U && total_weight == UINT64_MAX);
    assert(edges[0].weight == 99U && edges[1].weight == 99U);
    adjacency_list_destroy(graph);
}

int main(void) {
    test_adjacency_list_adapter();
    test_adjacency_matrix_adapter();
    test_empty_and_single_node_forests();
    test_disconnected_forest();
    test_directed_graph_rejected();
    test_total_weight_overflow();
    return 0;
}
