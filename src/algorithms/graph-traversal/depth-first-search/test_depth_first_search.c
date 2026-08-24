#include "depth_first_search.h"
#include "../../../data-structures/graphs/graph-view/graph_view.h"
#include "../../../data-structures/graphs/representations/adjacency-list/adjacency_list.h"
#include "../../../data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

#include <assert.h>
#include <stddef.h>

static void assert_weight_agnostic_traversal(const GraphView *view) {
    size_t order[4] = { 99U, 99U, 99U, 99U };
    size_t count = 0U;

    assert(depth_first_search(view, 0U, order, &count));
    assert(count == 3U);
    assert(order[0] == 0U);
    assert(order[1] == 1U);
    assert(order[2] == 2U);
    assert(order[3] == 99U);
}

static void test_adjacency_list_adapter(void) {
    int values[] = { 0, 1, 2, 3 };
    AdjacencyList *graph = adjacency_list_create(true);
    AdjacencyListNode *nodes[4] = { NULL };
    GraphView view = { 0 };
    size_t order[4] = { 0U };
    size_t count = 0U;

    assert(graph != NULL);
    for (size_t index = 0U; index < 4U; index++) {
        assert(adjacency_list_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_list_add_edge(graph, nodes[0], nodes[1], 99U));
    assert(adjacency_list_add_edge(graph, nodes[1], nodes[2], 1U));
    assert(adjacency_list_graph_view(graph, &view));
    assert_weight_agnostic_traversal(&view);
    assert(!depth_first_search(&view, 4U, order, &count));
    assert(!depth_first_search(&view, 0U, NULL, &count));
    assert(!depth_first_search(&view, 0U, order, NULL));
    adjacency_list_destroy(graph);
}

static void test_adjacency_matrix_adapter(void) {
    int values[] = { 0, 1, 2, 3 };
    AdjacencyMatrix *graph = adjacency_matrix_create(true);
    AdjacencyMatrixNode *nodes[4] = { NULL };
    GraphView view = { 0 };

    assert(graph != NULL);
    for (size_t index = 0U; index < 4U; index++) {
        assert(adjacency_matrix_add_node(graph, &values[index], &nodes[index]));
    }
    assert(adjacency_matrix_add_edge(graph, nodes[0], nodes[1], 99U));
    assert(adjacency_matrix_add_edge(graph, nodes[1], nodes[2], 1U));
    assert(adjacency_matrix_graph_view(graph, &view));
    assert_weight_agnostic_traversal(&view);
    adjacency_matrix_destroy(graph);
}

int main(void) {
    test_adjacency_list_adapter();
    test_adjacency_matrix_adapter();
    return 0;
}
