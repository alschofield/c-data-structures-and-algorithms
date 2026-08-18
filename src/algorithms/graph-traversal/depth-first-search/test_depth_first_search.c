#include "depth_first_search.h"

#include <assert.h>
#include <stddef.h>

static bool visited_exactly_once(const size_t *order, size_t count, size_t vertex) {
    size_t seen = 0U;

    for (size_t index = 0U; index < count; index++) {
        if (order[index] == vertex) {
            seen++;
        }
    }
    return seen == 1U;
}

static void test_path_graph_order_is_unambiguous(void) {
    AdjacencyList *graph = adjacency_list_create(4U, true);
    size_t order[4] = { 99U, 99U, 99U, 99U };
    size_t count = 0U;

    assert(graph != NULL);
    assert(adjacency_list_add_edge(graph, 0U, 1U));
    assert(adjacency_list_add_edge(graph, 1U, 2U));
    assert(adjacency_list_add_edge(graph, 2U, 3U));

    assert(depth_first_search(graph, 0U, order, &count));
    assert(count == 4U);
    assert(order[0] == 0U);
    assert(order[1] == 1U);
    assert(order[2] == 2U);
    assert(order[3] == 3U);
    adjacency_list_destroy(graph);
}

static void test_visits_reachable_exactly_once(void) {
    AdjacencyList *graph = adjacency_list_create(6U, true);
    size_t order[6] = { 99U, 99U, 99U, 99U, 99U, 99U };
    size_t count = 0U;

    assert(graph != NULL);
    assert(adjacency_list_add_edge(graph, 0U, 1U));
    assert(adjacency_list_add_edge(graph, 0U, 2U));
    assert(adjacency_list_add_edge(graph, 1U, 3U));
    assert(adjacency_list_add_edge(graph, 2U, 3U));
    assert(adjacency_list_add_edge(graph, 3U, 0U));
    assert(adjacency_list_add_edge(graph, 4U, 5U));

    assert(depth_first_search(graph, 0U, order, &count));
    assert(count == 4U);
    assert(order[0] == 0U);
    assert(visited_exactly_once(order, count, 1U));
    assert(visited_exactly_once(order, count, 2U));
    assert(visited_exactly_once(order, count, 3U));
    assert(!visited_exactly_once(order, count, 4U));
    assert(!visited_exactly_once(order, count, 5U));
    adjacency_list_destroy(graph);
}

static void test_self_loops_terminate(void) {
    AdjacencyList *graph = adjacency_list_create(2U, true);
    size_t order[2] = { 99U, 99U };
    size_t count = 0U;

    assert(graph != NULL);
    assert(adjacency_list_add_edge(graph, 0U, 0U));
    assert(adjacency_list_add_edge(graph, 0U, 1U));
    assert(adjacency_list_add_edge(graph, 1U, 1U));

    assert(depth_first_search(graph, 0U, order, &count));
    assert(count == 2U);
    assert(order[0] == 0U);
    assert(order[1] == 1U);
    adjacency_list_destroy(graph);
}

static void test_invalid_inputs(void) {
    AdjacencyList *graph = adjacency_list_create(2U, true);
    size_t order[2] = { 99U, 99U };
    size_t count = 99U;

    assert(graph != NULL);
    assert(!depth_first_search(graph, 2U, order, &count));
    assert(!depth_first_search(graph, 0U, NULL, &count));
    assert(!depth_first_search(graph, 0U, order, NULL));
    assert(!depth_first_search(NULL, 0U, order, &count));
    assert(order[0] == 99U);
    assert(count == 99U);
    adjacency_list_destroy(graph);
}

int main(void) {
    test_path_graph_order_is_unambiguous();
    test_visits_reachable_exactly_once();
    test_self_loops_terminate();
    test_invalid_inputs();
    return 0;
}
