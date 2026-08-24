#include "dijkstra.h"
#include "../../../data-structures/graphs/graph-view/graph_view.h"
#include "../../../data-structures/graphs/representations/adjacency-list/adjacency_list.h"
#include "../../../data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

static void assert_weighted_shortest_paths(const GraphView *view) {
    uint64_t distances[5] = { 0U };
    size_t parents[5] = { 0U };

    assert(dijkstra(view, 0U, distances, parents));
    assert(distances[0] == 0U);
    assert(distances[1] == 3U);
    assert(distances[2] == 1U);
    assert(distances[3] == 4U);
    assert(distances[4] == DIJKSTRA_INFINITY);
    assert(parents[0] == 0U);
    assert(parents[1] == 2U);
    assert(parents[2] == 0U);
    assert(parents[3] == 1U);
}

static void test_adjacency_list_adapter(void) {
    int values[] = { 0, 1, 2, 3, 4 };
    AdjacencyList *graph = adjacency_list_create(true);
    AdjacencyListNode *nodes[5] = { NULL };
    GraphView view = { 0 };
    uint64_t distances[5] = { 0U };
    size_t parents[5] = { 0U };

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
    assert_weighted_shortest_paths(&view);
    assert(!dijkstra(&view, 5U, distances, parents));
    assert(!dijkstra(&view, 0U, NULL, parents));
    assert(!dijkstra(&view, 0U, distances, NULL));
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
    assert_weighted_shortest_paths(&view);
    adjacency_matrix_destroy(graph);
}

int main(void) {
    test_adjacency_list_adapter();
    test_adjacency_matrix_adapter();
    return 0;
}
