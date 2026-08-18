#include "adjacency_matrix.h"

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

static void test_directed_add_remove(void) {
    AdjacencyMatrix *graph = adjacency_matrix_create(3U, true);

    assert(graph != NULL);
    assert(adjacency_matrix_vertex_count(graph) == 3U);
    assert(adjacency_matrix_edge_count(graph) == 0U);
    assert(adjacency_matrix_add_edge(graph, 0U, 1U));
    assert(adjacency_matrix_has_edge(graph, 0U, 1U));
    assert(!adjacency_matrix_has_edge(graph, 1U, 0U));
    assert(adjacency_matrix_edge_count(graph) == 1U);

    assert(adjacency_matrix_remove_edge(graph, 0U, 1U));
    assert(!adjacency_matrix_has_edge(graph, 0U, 1U));
    assert(adjacency_matrix_edge_count(graph) == 0U);
    adjacency_matrix_destroy(graph);
}

static void test_undirected_symmetry(void) {
    AdjacencyMatrix *graph = adjacency_matrix_create(3U, false);

    assert(graph != NULL);
    assert(adjacency_matrix_add_edge(graph, 0U, 2U));
    assert(adjacency_matrix_has_edge(graph, 0U, 2U));
    assert(adjacency_matrix_has_edge(graph, 2U, 0U));

    assert(adjacency_matrix_remove_edge(graph, 2U, 0U));
    assert(!adjacency_matrix_has_edge(graph, 0U, 2U));
    assert(!adjacency_matrix_has_edge(graph, 2U, 0U));
    adjacency_matrix_destroy(graph);
}

static void test_duplicate_and_absent_edges_are_noops(void) {
    AdjacencyMatrix *graph = adjacency_matrix_create(2U, true);

    assert(graph != NULL);
    assert(adjacency_matrix_add_edge(graph, 0U, 1U));
    assert(!adjacency_matrix_add_edge(graph, 0U, 1U));
    assert(adjacency_matrix_edge_count(graph) == 1U);
    assert(!adjacency_matrix_remove_edge(graph, 1U, 0U));
    assert(adjacency_matrix_edge_count(graph) == 1U);
    adjacency_matrix_destroy(graph);
}

static void test_neighbor_iteration_scans_row(void) {
    AdjacencyMatrix *graph = adjacency_matrix_create(4U, true);
    struct NeighborLog log = { .count = 0U };

    assert(graph != NULL);
    assert(adjacency_matrix_add_edge(graph, 2U, 0U));
    assert(adjacency_matrix_add_edge(graph, 2U, 3U));

    assert(adjacency_matrix_neighbors(graph, 2U, record_neighbor, &log));
    assert(log.count == 2U);
    assert(log.visited[0] == 0U);
    assert(log.visited[1] == 3U);
    adjacency_matrix_destroy(graph);
}

static void test_out_of_range_and_null(void) {
    AdjacencyMatrix *graph = adjacency_matrix_create(2U, true);
    struct NeighborLog log = { .count = 0U };

    assert(graph != NULL);
    assert(!adjacency_matrix_add_edge(graph, 0U, 2U));
    assert(!adjacency_matrix_remove_edge(graph, 2U, 0U));
    assert(!adjacency_matrix_has_edge(graph, 0U, 2U));
    assert(!adjacency_matrix_neighbors(graph, 2U, record_neighbor, &log));
    adjacency_matrix_destroy(graph);

    assert(!adjacency_matrix_add_edge(NULL, 0U, 1U));
    assert(!adjacency_matrix_has_edge(NULL, 0U, 1U));
    assert(adjacency_matrix_vertex_count(NULL) == 0U);
    assert(adjacency_matrix_edge_count(NULL) == 0U);
    adjacency_matrix_destroy(NULL);
}

int main(void) {
    test_directed_add_remove();
    test_undirected_symmetry();
    test_duplicate_and_absent_edges_are_noops();
    test_neighbor_iteration_scans_row();
    test_out_of_range_and_null();
    return 0;
}
