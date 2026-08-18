#include "adjacency_list.h"

#include <assert.h>
#include <stddef.h>

struct NeighborLog {
    size_t visited[8];
    size_t count;
};

static bool record_neighbor(size_t neighbor, void *context) {
    struct NeighborLog *log = context;

    log->visited[log->count] = neighbor;
    log->count++;
    return true;
}

static bool stop_after_first(size_t neighbor, void *context) {
    struct NeighborLog *log = context;

    log->visited[log->count] = neighbor;
    log->count++;
    return false;
}

static void test_directed_edges(void) {
    AdjacencyList *graph = adjacency_list_create(4U, true);

    assert(graph != NULL);
    assert(adjacency_list_vertex_count(graph) == 4U);
    assert(adjacency_list_edge_count(graph) == 0U);
    assert(adjacency_list_add_edge(graph, 0U, 1U));
    assert(adjacency_list_add_edge(graph, 0U, 2U));
    assert(adjacency_list_add_edge(graph, 3U, 0U));
    assert(adjacency_list_edge_count(graph) == 3U);

    assert(adjacency_list_has_edge(graph, 0U, 1U));
    assert(!adjacency_list_has_edge(graph, 1U, 0U));
    assert(adjacency_list_has_edge(graph, 3U, 0U));
    assert(!adjacency_list_has_edge(graph, 0U, 3U));
    adjacency_list_destroy(graph);
}

static void test_undirected_edges(void) {
    AdjacencyList *graph = adjacency_list_create(3U, false);

    assert(graph != NULL);
    assert(adjacency_list_add_edge(graph, 0U, 1U));
    assert(adjacency_list_has_edge(graph, 0U, 1U));
    assert(adjacency_list_has_edge(graph, 1U, 0U));
    adjacency_list_destroy(graph);
}

static void test_neighbor_iteration(void) {
    AdjacencyList *graph = adjacency_list_create(4U, true);
    struct NeighborLog log = { { 0U }, 0U };

    assert(graph != NULL);
    assert(adjacency_list_add_edge(graph, 1U, 0U));
    assert(adjacency_list_add_edge(graph, 1U, 2U));
    assert(adjacency_list_add_edge(graph, 1U, 3U));

    assert(adjacency_list_neighbors(graph, 1U, record_neighbor, &log));
    assert(log.count == 3U);

    log.count = 0U;
    assert(!adjacency_list_neighbors(graph, 1U, stop_after_first, &log));
    assert(log.count == 1U);

    log.count = 0U;
    assert(adjacency_list_neighbors(graph, 0U, record_neighbor, &log));
    assert(log.count == 0U);
    adjacency_list_destroy(graph);
}

static void test_out_of_range_vertexes(void) {
    AdjacencyList *graph = adjacency_list_create(2U, true);
    struct NeighborLog log = { { 0U }, 0U };

    assert(graph != NULL);
    assert(!adjacency_list_add_edge(graph, 0U, 2U));
    assert(!adjacency_list_add_edge(graph, 2U, 0U));
    assert(!adjacency_list_has_edge(graph, 0U, 2U));
    assert(!adjacency_list_neighbors(graph, 2U, record_neighbor, &log));
    assert(adjacency_list_edge_count(graph) == 0U);
    adjacency_list_destroy(graph);
}

static void test_null_container(void) {
    struct NeighborLog log = { { 0U }, 0U };

    assert(!adjacency_list_add_edge(NULL, 0U, 1U));
    assert(!adjacency_list_has_edge(NULL, 0U, 1U));
    assert(!adjacency_list_neighbors(NULL, 0U, record_neighbor, &log));
    assert(adjacency_list_vertex_count(NULL) == 0U);
    assert(adjacency_list_edge_count(NULL) == 0U);
    adjacency_list_destroy(NULL);
}

int main(void) {
    test_directed_edges();
    test_undirected_edges();
    test_neighbor_iteration();
    test_out_of_range_vertexes();
    test_null_container();
    return 0;
}
