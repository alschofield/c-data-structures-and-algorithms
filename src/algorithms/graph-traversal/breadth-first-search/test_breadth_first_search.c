#include "breadth_first_search.h"

#include <assert.h>
#include <stddef.h>

static void test_level_order_from_source(void) {
    AdjacencyList *graph = adjacency_list_create(6U, true);
    size_t order[6] = { 99U, 99U, 99U, 99U, 99U, 99U };
    size_t count = 0U;

    assert(graph != NULL);
    assert(adjacency_list_add_edge(graph, 0U, 1U));
    assert(adjacency_list_add_edge(graph, 0U, 2U));
    assert(adjacency_list_add_edge(graph, 1U, 3U));
    assert(adjacency_list_add_edge(graph, 2U, 3U));
    assert(adjacency_list_add_edge(graph, 3U, 4U));

    assert(breadth_first_search(graph, 0U, order, &count));
    assert(count == 5U);
    assert(order[0] == 0U);
    assert(order[1] == 1U);
    assert(order[2] == 2U);
    assert(order[3] == 3U);
    assert(order[4] == 4U);
    assert(order[5] == 99U);
    adjacency_list_destroy(graph);
}

static void test_cycles_and_self_loops_terminate(void) {
    AdjacencyList *graph = adjacency_list_create(3U, true);
    size_t order[3] = { 99U, 99U, 99U };
    size_t count = 0U;

    assert(graph != NULL);
    assert(adjacency_list_add_edge(graph, 0U, 0U));
    assert(adjacency_list_add_edge(graph, 0U, 1U));
    assert(adjacency_list_add_edge(graph, 1U, 2U));
    assert(adjacency_list_add_edge(graph, 2U, 0U));

    assert(breadth_first_search(graph, 0U, order, &count));
    assert(count == 3U);
    assert(order[0] == 0U);
    assert(order[1] == 1U);
    assert(order[2] == 2U);
    adjacency_list_destroy(graph);
}

static void test_unreachable_vertices_not_visited(void) {
    AdjacencyList *graph = adjacency_list_create(4U, true);
    size_t order[4] = { 99U, 99U, 99U, 99U };
    size_t count = 0U;

    assert(graph != NULL);
    assert(adjacency_list_add_edge(graph, 0U, 1U));
    assert(adjacency_list_add_edge(graph, 2U, 3U));

    assert(breadth_first_search(graph, 0U, order, &count));
    assert(count == 2U);
    assert(order[0] == 0U);
    assert(order[1] == 1U);
    assert(order[2] == 99U);
    adjacency_list_destroy(graph);
}

static void test_invalid_inputs(void) {
    AdjacencyList *graph = adjacency_list_create(2U, true);
    size_t order[2] = { 99U, 99U };
    size_t count = 99U;

    assert(graph != NULL);
    assert(!breadth_first_search(graph, 2U, order, &count));
    assert(!breadth_first_search(graph, 0U, NULL, &count));
    assert(!breadth_first_search(graph, 0U, order, NULL));
    assert(!breadth_first_search(NULL, 0U, order, &count));
    assert(order[0] == 99U);
    assert(count == 99U);
    adjacency_list_destroy(graph);
}

int main(void) {
    test_level_order_from_source();
    test_cycles_and_self_loops_terminate();
    test_unreachable_vertices_not_visited();
    test_invalid_inputs();
    return 0;
}
