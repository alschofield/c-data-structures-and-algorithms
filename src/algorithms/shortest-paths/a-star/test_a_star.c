#include "a_star.h"
#include "../../../data-structures/graphs/graph-view/graph_view.h"
#include "../../../data-structures/graphs/representations/adjacency-list/adjacency_list.h"
#include "../../../data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

static uint64_t zero_heuristic(size_t vertex, void *context) {
    (void)vertex;
    (void)context;
    return 0U;
}

static uint64_t table_heuristic(size_t vertex, void *context) {
    const uint64_t *table = context;

    return table[vertex];
}

static void assert_weighted_shortest_path(const GraphView *view) {
    uint64_t remaining_costs[5] = { 4U, 1U, 3U, 0U, 0U };
    size_t path[5] = { 99U, 99U, 99U, 99U, 99U };
    size_t path_length = 0U;

    assert(a_star(view, 0U, 3U, zero_heuristic, NULL, path, &path_length));
    assert(path_length == 4U);
    assert(path[0] == 0U);
    assert(path[1] == 2U);
    assert(path[2] == 1U);
    assert(path[3] == 3U);

    assert(a_star(view, 0U, 3U, table_heuristic, remaining_costs, path, &path_length));
    assert(path_length == 4U);
    assert(path[0] == 0U);
    assert(path[1] == 2U);
    assert(path[2] == 1U);
    assert(path[3] == 3U);

    assert(a_star(view, 2U, 2U, zero_heuristic, NULL, path, &path_length));
    assert(path_length == 1U);
    assert(path[0] == 2U);

    path_length = 99U;
    assert(!a_star(view, 0U, 4U, zero_heuristic, NULL, path, &path_length));
    assert(path_length == 99U);
}

static void test_adjacency_list_adapter(void) {
    int values[] = { 0, 1, 2, 3, 4 };
    AdjacencyList *graph = adjacency_list_create(true);
    AdjacencyListNode *nodes[5] = { NULL };
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
    assert_weighted_shortest_path(&view);
    assert(!a_star(&view, 5U, 3U, zero_heuristic, NULL, path, &path_length));
    assert(!a_star(&view, 0U, 5U, zero_heuristic, NULL, path, &path_length));
    assert(!a_star(&view, 0U, 3U, NULL, NULL, path, &path_length));
    assert(!a_star(&view, 0U, 3U, zero_heuristic, NULL, NULL, &path_length));
    assert(!a_star(&view, 0U, 3U, zero_heuristic, NULL, path, NULL));
    adjacency_list_destroy(graph);
}

static void test_adjacency_matrix_adapter(void) {
    int values[] = { 0, 1, 2, 3, 4 };
    AdjacencyMatrix *graph = adjacency_matrix_create(true);
    AdjacencyMatrixNode *nodes[5] = { NULL };
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
    assert_weighted_shortest_path(&view);
    adjacency_matrix_destroy(graph);
}

int main(void) {
    test_adjacency_list_adapter();
    test_adjacency_matrix_adapter();
    return 0;
}
