#include "kruskal.h"
#include "../../../data-structures/graphs/graph-view/graph_view.h"
#include "../../../data-structures/graphs/representations/adjacency-list/adjacency_list.h"
#include "../../../data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

static void assert_minimum_spanning_tree(const GraphView *view) {
    KruskalEdge edges[3] = { 0 };
    size_t edge_count = 99U;
    uint64_t total_weight = UINT64_MAX;

    assert(kruskal_minimum_spanning_forest(
        view, edges, 3U, &edge_count, &total_weight
    ));
    assert(edge_count == 3U);
    assert(total_weight == 6U);
    assert(edges[0].weight == 1U);
    assert(edges[1].weight == 2U);
    assert(edges[2].weight == 3U);
}

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

int main(void) {
    test_adjacency_list_adapter();
    test_adjacency_matrix_adapter();
    return 0;
}
