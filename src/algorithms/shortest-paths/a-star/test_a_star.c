// Declares the public A-star API under test.
#include "a_star.h"
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
// Declares uint64_t.
#include <stdint.h>

// Supplies a zero estimate so A-star must behave as goal-directed Dijkstra.
static uint64_t zero_heuristic(size_t vertex, void *context) {
    (void)vertex;
    (void)context;
    return 0U;
}

// Looks up a caller-provided admissible estimate by dense Node index.
static uint64_t table_heuristic(size_t vertex, void *context) {
    const uint64_t *table = context;

    return table[vertex];
}

// Verifies both zero and informed heuristics reconstruct the weighted best path.
static void assert_weighted_shortest_path(const GraphView *view, Node *nodes[]) {
    uint64_t remaining_costs[5] = { 4U, 1U, 3U, 0U, 0U };
    size_t path[5] = { 99U, 99U, 99U, 99U, 99U };
    size_t path_length = 0U;
    size_t too_small_path[3] = { 77U, 77U, 77U };
    size_t too_small_length = 99U;

    assert(a_star(view, nodes[0], nodes[3], zero_heuristic, NULL, path, 5U, &path_length));
    assert(path_length == 4U);
    assert(path[0] == 0U);
    assert(path[1] == 2U);
    assert(path[2] == 1U);
    assert(path[3] == 3U);

    assert(a_star(view, nodes[0], nodes[3], table_heuristic, remaining_costs, path, 5U, &path_length));
    assert(path_length == 4U);
    assert(path[0] == 0U);
    assert(path[1] == 2U);
    assert(path[2] == 1U);
    assert(path[3] == 3U);

    assert(a_star(view, nodes[2], nodes[2], zero_heuristic, NULL, path, 5U, &path_length));
    assert(path_length == 1U);
    assert(path[0] == 2U);

    assert(!a_star(view, nodes[0], nodes[3], zero_heuristic, NULL, too_small_path, 3U, &too_small_length));
    assert(too_small_length == 99U);
    assert(too_small_path[0] == 77U && too_small_path[1] == 77U && too_small_path[2] == 77U);

    path_length = 99U;
    assert(!a_star(view, nodes[0], nodes[4], zero_heuristic, NULL, path, 5U, &path_length));
    assert(path_length == 99U);
}

// Verifies a lower g-score re-expands a Node under an inconsistent heuristic.
static void assert_inconsistent_heuristic_path(const GraphView *view, Node *nodes[]) {
    uint64_t heuristic[4] = { 0U, 0U, 101U, 0U };
    size_t path[4] = { 99U, 99U, 99U, 99U };
    size_t path_length = 0U;

    assert(a_star(view, nodes[0], nodes[3], table_heuristic, heuristic, path, 4U, &path_length));
    assert(path_length == 4U);
    assert(path[0] == 0U);
    assert(path[1] == 2U);
    assert(path[2] == 1U);
    assert(path[3] == 3U);
}

// Builds the shared weighted-path test fixture through the adjacency-list adapter.
static void test_adjacency_list_adapter(void) {
    int values[] = { 0, 1, 2, 3, 4 };
    AdjacencyList *graph = adjacency_list_create(true);
    Node *nodes[5] = { NULL };
    Node invalid_node = { .index = 5U };
    GraphView view = { 0 };
    size_t path[5] = { 0U };
    size_t path_length = 0U;

    assert(graph != NULL);
    for (size_t index = 0U; index < 5U; index++) {
        assert(adjacency_list_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[1], 4U));
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[2], 1U));
    assert(adjacency_list_add_edge(graph, nodes[2], nodes[1], 2U));
    assert(adjacency_list_add_edge(graph, nodes[1], nodes[3], 1U));
    assert(adjacency_list_add_edge(graph, nodes[2], nodes[3], 5U));
    assert(adjacency_list_graph_view(graph, &view));

    assert_weighted_shortest_path(&view, nodes);
    assert(!a_star(&view, &invalid_node, nodes[3], zero_heuristic, NULL, path, 5U, &path_length));
    assert(!a_star(&view, nodes[0], &invalid_node, zero_heuristic, NULL, path, 5U, &path_length));
    assert(!a_star(&view, NULL, nodes[3], zero_heuristic, NULL, path, 5U, &path_length));
    assert(!a_star(&view, nodes[0], NULL, zero_heuristic, NULL, path, 5U, &path_length));
    assert(!a_star(&view, nodes[0], nodes[3], NULL, NULL, path, 5U, &path_length));
    assert(!a_star(&view, nodes[0], nodes[3], zero_heuristic, NULL, NULL, 5U, &path_length));
    assert(!a_star(&view, nodes[0], nodes[3], zero_heuristic, NULL, path, 5U, NULL));
    assert(!a_star(&view, nodes[0], nodes[3], zero_heuristic, NULL, path, 0U, &path_length));
    adjacency_list_destroy(graph);
}

// Builds the shared weighted-path test fixture through the adjacency-matrix adapter.
static void test_adjacency_matrix_adapter(void) {
    int values[] = { 0, 1, 2, 3, 4 };
    AdjacencyMatrix *graph = adjacency_matrix_create(true);
    Node *nodes[5] = { NULL };
    GraphView view = { 0 };

    assert(graph != NULL);
    for (size_t index = 0U; index < 5U; index++) {
        assert(adjacency_matrix_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[1], 4U));
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[2], 1U));
    assert(adjacency_matrix_add_edge(graph, nodes[2], nodes[1], 2U));
    assert(adjacency_matrix_add_edge(graph, nodes[1], nodes[3], 1U));
    assert(adjacency_matrix_add_edge(graph, nodes[2], nodes[3], 5U));
    assert(adjacency_matrix_graph_view(graph, &view));

    assert_weighted_shortest_path(&view, nodes);
    adjacency_matrix_destroy(graph);
}

// Builds an adjacency-list graph where an inconsistent heuristic requires re-expansion.
static void test_inconsistent_heuristic_with_adjacency_list(void) {
    int values[] = { 0, 1, 2, 3 };
    AdjacencyList *graph = adjacency_list_create(true);
    Node *nodes[4] = { NULL };
    GraphView view = { 0 };

    assert(graph != NULL);
    for (size_t index = 0U; index < 4U; index++) {
        assert(adjacency_list_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[1], 5U));
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[2], 1U));
    assert(adjacency_list_add_edge(graph, nodes[2], nodes[1], 1U));
    assert(adjacency_list_add_edge(graph, nodes[1], nodes[3], 100U));
    assert(adjacency_list_graph_view(graph, &view));

    assert_inconsistent_heuristic_path(&view, nodes);
    adjacency_list_destroy(graph);
}

// Builds the same re-expansion graph through the adjacency-matrix adapter.
static void test_inconsistent_heuristic_with_adjacency_matrix(void) {
    int values[] = { 0, 1, 2, 3 };
    AdjacencyMatrix *graph = adjacency_matrix_create(true);
    Node *nodes[4] = { NULL };
    GraphView view = { 0 };

    assert(graph != NULL);
    for (size_t index = 0U; index < 4U; index++) {
        assert(adjacency_matrix_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[1], 5U));
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[2], 1U));
    assert(adjacency_matrix_add_edge(graph, nodes[2], nodes[1], 1U));
    assert(adjacency_matrix_add_edge(graph, nodes[1], nodes[3], 100U));
    assert(adjacency_matrix_graph_view(graph, &view));

    assert_inconsistent_heuristic_path(&view, nodes);
    adjacency_matrix_destroy(graph);
}

int main(void) {
    test_adjacency_list_adapter();
    test_adjacency_matrix_adapter();
    test_inconsistent_heuristic_with_adjacency_list();
    test_inconsistent_heuristic_with_adjacency_matrix();
    return 0;
}
