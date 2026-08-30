// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public Dijkstra API.
#include "../src/algorithms/shortest-paths/dijkstra/dijkstra.h"
// Provides the adjacency-list GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-list/adjacency_list.h"
// Provides the adjacency-matrix GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

// Limits sparse graph setup and full shortest-path samples to a practical size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 2000U
#endif

// Defines the number of reachable Nodes in list-backed samples.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };
// Keeps dense matrix setup practical while still measuring a substantial graph.
enum { MATRIX_ITEM_COUNT = 1000U };

// Stores state shared by adjacency-list Dijkstra benchmark samples.
struct DijkstraContext {
    AdjacencyList *graph;
    int values[ITEM_COUNT];
    Node *nodes[ITEM_COUNT];
    GraphView view;
    uint64_t distances[ITEM_COUNT];
    size_t parents[ITEM_COUNT];
};

// Stores state shared by adjacency-matrix Dijkstra benchmark samples.
struct MatrixDijkstraContext {
    AdjacencyMatrix *graph;
    int values[MATRIX_ITEM_COUNT];
    Node *nodes[MATRIX_ITEM_COUNT];
    GraphView view;
    uint64_t distances[MATRIX_ITEM_COUNT];
    size_t parents[MATRIX_ITEM_COUNT];
};

// Builds a unit-weight directed adjacency-list chain outside the timed operation.
static bool setup(void *context) {
    struct DijkstraContext *dijkstra_context = context;

    dijkstra_context->graph = adjacency_list_create(true);
    if (dijkstra_context->graph == NULL) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!adjacency_list_add_node(dijkstra_context->graph, &dijkstra_context->values[index], &dijkstra_context->nodes[index])) {
            adjacency_list_destroy(dijkstra_context->graph);
            dijkstra_context->graph = NULL;
            return false;
        }
    }

    for (size_t index = 0U; index + 1U < ITEM_COUNT; index++) {
        if (!adjacency_list_add_edge(dijkstra_context->graph, dijkstra_context->nodes[index], dijkstra_context->nodes[index + 1U], 1U)) {
            adjacency_list_destroy(dijkstra_context->graph);
            dijkstra_context->graph = NULL;
            return false;
        }
    }

    if (!adjacency_list_graph_view(dijkstra_context->graph, &dijkstra_context->view)) {
        adjacency_list_destroy(dijkstra_context->graph);
        dijkstra_context->graph = NULL;
        return false;
    }

    return true;
}

// Computes all shortest paths from the list chain source as one timed operation.
static bool dijkstra_operation(void *context) {
    struct DijkstraContext *dijkstra_context = context;

    return dijkstra(&dijkstra_context->view, dijkstra_context->nodes[0], dijkstra_context->distances, dijkstra_context->parents);
}

// Confirms the final chain Node receives its expected unit-edge distance.
static bool verify(void *context) {
    struct DijkstraContext *dijkstra_context = context;

    return dijkstra_context->distances[ITEM_COUNT - 1U] == ITEM_COUNT - 1U &&
        dijkstra_context->parents[ITEM_COUNT - 1U] == ITEM_COUNT - 2U;
}

// Releases the adjacency-list graph created for one benchmark sample.
static void teardown(void *context) {
    struct DijkstraContext *dijkstra_context = context;

    adjacency_list_destroy(dijkstra_context->graph);
    dijkstra_context->graph = NULL;
}

// Builds a unit-weight directed adjacency-matrix chain outside the timed operation.
static bool matrix_setup(void *context) {
    struct MatrixDijkstraContext *dijkstra_context = context;

    dijkstra_context->graph = adjacency_matrix_create(true);
    if (dijkstra_context->graph == NULL) {
        return false;
    }

    for (size_t index = 0U; index < MATRIX_ITEM_COUNT; index++) {
        if (!adjacency_matrix_add_node(dijkstra_context->graph, &dijkstra_context->values[index], &dijkstra_context->nodes[index])) {
            adjacency_matrix_destroy(dijkstra_context->graph);
            dijkstra_context->graph = NULL;
            return false;
        }
    }

    for (size_t index = 0U; index + 1U < MATRIX_ITEM_COUNT; index++) {
        if (!adjacency_matrix_add_edge(dijkstra_context->graph, dijkstra_context->nodes[index], dijkstra_context->nodes[index + 1U], 1U)) {
            adjacency_matrix_destroy(dijkstra_context->graph);
            dijkstra_context->graph = NULL;
            return false;
        }
    }

    if (!adjacency_matrix_graph_view(dijkstra_context->graph, &dijkstra_context->view)) {
        adjacency_matrix_destroy(dijkstra_context->graph);
        dijkstra_context->graph = NULL;
        return false;
    }

    return true;
}

// Computes all shortest paths from the matrix chain source as one timed operation.
static bool matrix_dijkstra_operation(void *context) {
    struct MatrixDijkstraContext *dijkstra_context = context;

    return dijkstra(&dijkstra_context->view, dijkstra_context->nodes[0], dijkstra_context->distances, dijkstra_context->parents);
}

// Confirms the final matrix Node receives its expected unit-edge distance.
static bool matrix_verify(void *context) {
    struct MatrixDijkstraContext *dijkstra_context = context;

    return dijkstra_context->distances[MATRIX_ITEM_COUNT - 1U] == MATRIX_ITEM_COUNT - 1U &&
        dijkstra_context->parents[MATRIX_ITEM_COUNT - 1U] == MATRIX_ITEM_COUNT - 2U;
}

// Releases the adjacency-matrix graph created for one benchmark sample.
static void matrix_teardown(void *context) {
    struct MatrixDijkstraContext *dijkstra_context = context;

    adjacency_matrix_destroy(dijkstra_context->graph);
    dijkstra_context->graph = NULL;
}

// Runs and prints one configured Dijkstra benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct DijkstraContext dijkstra_context;
    static struct MatrixDijkstraContext matrix_dijkstra_context;
    BenchmarkConfig config = {
        .name = "Dijkstra 2000-node adjacency-list chain",
        .warmup_iterations = 1U,
        .sample_count = 21U,
        .iterations_per_sample = 1U,
        .setup = setup,
        .operation = dijkstra_operation,
        .verify = verify,
        .teardown = teardown,
        .context = &dijkstra_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        dijkstra_context.values[index] = (int)index;
    }
    for (size_t index = 0U; index < MATRIX_ITEM_COUNT; index++) {
        matrix_dijkstra_context.values[index] = (int)index;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Dijkstra 1000-node adjacency-matrix chain";
    config.context = &matrix_dijkstra_context;
    config.setup = matrix_setup;
    config.operation = matrix_dijkstra_operation;
    config.verify = matrix_verify;
    config.teardown = matrix_teardown;
    return run_benchmark(&config) ? 0 : 1;
}
