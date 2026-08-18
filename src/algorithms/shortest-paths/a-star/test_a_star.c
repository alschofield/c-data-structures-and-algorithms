#include "a_star.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

static const WeightedGraphEdge EDGES_FROM_0[] = {
    { .to = 1U, .weight = 4U }, { .to = 2U, .weight = 1U }
};
static const WeightedGraphEdge EDGES_FROM_1[] = { { .to = 3U, .weight = 1U } };
static const WeightedGraphEdge EDGES_FROM_2[] = {
    { .to = 1U, .weight = 2U }, { .to = 3U, .weight = 5U }
};
static const WeightedGraphEdge *const EDGE_TABLE[] = {
    EDGES_FROM_0, EDGES_FROM_1, EDGES_FROM_2, NULL, NULL
};
static const size_t DEGREES[] = { 2U, 1U, 2U, 0U, 0U };

static uint64_t zero_heuristic(size_t vertex, void *context) {
    (void)vertex;
    (void)context;
    return 0U;
}

static uint64_t table_heuristic(size_t vertex, void *context) {
    const uint64_t *table = context;

    return table[vertex];
}

static void test_zero_heuristic_matches_dijkstra(void) {
    WeightedGraph graph = {
        .vertex_count = 5U, .edges = EDGE_TABLE, .degrees = DEGREES
    };
    size_t path[5] = { 99U, 99U, 99U, 99U, 99U };
    size_t path_length = 0U;

    assert(a_star(&graph, 0U, 3U, zero_heuristic, NULL, path, &path_length));
    assert(path_length == 4U);
    assert(path[0] == 0U);
    assert(path[1] == 2U);
    assert(path[2] == 1U);
    assert(path[3] == 3U);
}

static void test_admissible_heuristic_stays_optimal(void) {
    WeightedGraph graph = {
        .vertex_count = 5U, .edges = EDGE_TABLE, .degrees = DEGREES
    };
    uint64_t remaining_costs[5] = { 4U, 1U, 3U, 0U, 0U };
    size_t path[5] = { 99U, 99U, 99U, 99U, 99U };
    size_t path_length = 0U;

    assert(a_star(&graph, 0U, 3U, table_heuristic, remaining_costs, path, &path_length));
    assert(path_length == 4U);
    assert(path[0] == 0U);
    assert(path[1] == 2U);
    assert(path[2] == 1U);
    assert(path[3] == 3U);
}

static void test_source_equals_goal(void) {
    WeightedGraph graph = {
        .vertex_count = 5U, .edges = EDGE_TABLE, .degrees = DEGREES
    };
    size_t path[5] = { 99U, 99U, 99U, 99U, 99U };
    size_t path_length = 0U;

    assert(a_star(&graph, 2U, 2U, zero_heuristic, NULL, path, &path_length));
    assert(path_length == 1U);
    assert(path[0] == 2U);
}

static void test_unreachable_goal_is_no_path(void) {
    WeightedGraph graph = {
        .vertex_count = 5U, .edges = EDGE_TABLE, .degrees = DEGREES
    };
    size_t path[5] = { 99U, 99U, 99U, 99U, 99U };
    size_t path_length = 99U;

    assert(!a_star(&graph, 0U, 4U, zero_heuristic, NULL, path, &path_length));
    assert(path[0] == 99U);
    assert(path_length == 99U);
}

static void test_invalid_inputs(void) {
    WeightedGraph graph = {
        .vertex_count = 5U, .edges = EDGE_TABLE, .degrees = DEGREES
    };
    size_t path[5] = { 99U, 99U, 99U, 99U, 99U };
    size_t path_length = 99U;

    assert(!a_star(&graph, 5U, 3U, zero_heuristic, NULL, path, &path_length));
    assert(!a_star(&graph, 0U, 5U, zero_heuristic, NULL, path, &path_length));
    assert(!a_star(&graph, 0U, 3U, NULL, NULL, path, &path_length));
    assert(!a_star(&graph, 0U, 3U, zero_heuristic, NULL, NULL, &path_length));
    assert(!a_star(&graph, 0U, 3U, zero_heuristic, NULL, path, NULL));
    assert(!a_star(NULL, 0U, 3U, zero_heuristic, NULL, path, &path_length));
}

int main(void) {
    test_zero_heuristic_matches_dijkstra();
    test_admissible_heuristic_stays_optimal();
    test_source_equals_goal();
    test_unreachable_goal_is_no_path();
    test_invalid_inputs();
    return 0;
}
