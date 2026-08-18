#include "dijkstra.h"

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

static void test_shortest_distances_and_parents(void) {
    WeightedGraph graph = {
        .vertex_count = 5U, .edges = EDGE_TABLE, .degrees = DEGREES
    };
    uint64_t distances[5] = { 0U };
    size_t parents[5] = { 0U };

    assert(dijkstra(&graph, 0U, distances, parents));
    assert(distances[0] == 0U);
    assert(distances[1] == 3U);
    assert(distances[2] == 1U);
    assert(distances[3] == 4U);
    assert(distances[4] == DIJKSTRA_INFINITY);

    assert(parents[0] == 0U);
    assert(parents[1] == 2U);
    assert(parents[2] == 0U);
    assert(parents[3] == 1U);
    assert(parents[4] == DIJKSTRA_NO_PARENT);
}

static void test_single_vertex_graph(void) {
    WeightedGraph graph = {
        .vertex_count = 1U, .edges = EDGE_TABLE, .degrees = DEGREES
    };
    uint64_t distances[1] = { 99U };
    size_t parents[1] = { 99U };

    assert(dijkstra(&graph, 0U, distances, parents));
    assert(distances[0] == 0U);
    assert(parents[0] == 0U);
}

static void test_invalid_inputs(void) {
    WeightedGraph graph = {
        .vertex_count = 5U, .edges = EDGE_TABLE, .degrees = DEGREES
    };
    uint64_t distances[5] = { 0U };
    size_t parents[5] = { 0U };

    assert(!dijkstra(&graph, 5U, distances, parents));
    assert(!dijkstra(&graph, 0U, NULL, parents));
    assert(!dijkstra(&graph, 0U, distances, NULL));
    assert(!dijkstra(NULL, 0U, distances, parents));
}

int main(void) {
    test_shortest_distances_and_parents();
    test_single_vertex_graph();
    test_invalid_inputs();
    return 0;
}
