// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public A-star API.
#include "../src/algorithms/shortest-paths/a-star/a_star.h"
// Provides the adjacency-list GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-list/adjacency_list.h"
// Provides the adjacency-matrix GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

// Limits sparse graph setup and full pathfinding samples to a practical size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 2000U
#endif

// Defines the number of Nodes in list-backed benchmark paths.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };
// Keeps dense matrix setup practical while still measuring a substantial path.
enum { MATRIX_ITEM_COUNT = 1000U };

// Stores state shared by adjacency-list A-star benchmark samples.
struct AStarContext {
    // Owns the graph built outside each timed sample.
    AdjacencyList *graph;
    // Stores caller-owned Node payload values.
    int values[ITEM_COUNT];
    // Stores graph-owned Node handles in chain order.
    Node *nodes[ITEM_COUNT];
    // Holds the representation-independent graph adapter.
    GraphView view;
    // Receives the source-to-goal index path from each operation.
    size_t path[ITEM_COUNT];
    // Receives the number of indexes written into path.
    size_t path_length;
};

// Stores state shared by adjacency-matrix A-star benchmark samples.
struct MatrixAStarContext {
    // Owns the graph built outside each timed sample.
    AdjacencyMatrix *graph;
    // Stores caller-owned Node payload values.
    int values[MATRIX_ITEM_COUNT];
    // Stores graph-owned Node handles in chain order.
    Node *nodes[MATRIX_ITEM_COUNT];
    // Holds the representation-independent graph adapter.
    GraphView view;
    // Receives the source-to-goal index path from each operation.
    size_t path[MATRIX_ITEM_COUNT];
    // Receives the number of indexes written into path.
    size_t path_length;
};

// Supplies a zero estimate so the benchmark measures the Dijkstra-equivalent baseline.
static uint64_t zero_heuristic(size_t vertex, void *context) {
    (void)vertex;
    (void)context;
    return 0U;
}

// Builds a unit-weight directed adjacency-list chain outside the timed operation.
static bool setup(void *context) {
    struct AStarContext *a_star_context = context;

    a_star_context->graph = adjacency_list_create(true);
    if (a_star_context->graph == NULL) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!adjacency_list_add_node(
                a_star_context->graph,
                &a_star_context->values[index],
                &a_star_context->nodes[index]
            )) {
            adjacency_list_destroy(a_star_context->graph);
            a_star_context->graph = NULL;
            return false;
        }
    }

    for (size_t index = 0U; index + 1U < ITEM_COUNT; index++) {
        if (!adjacency_list_add_edge(
                a_star_context->graph,
                a_star_context->nodes[index],
                a_star_context->nodes[index + 1U],
                1U
            )) {
            adjacency_list_destroy(a_star_context->graph);
            a_star_context->graph = NULL;
            return false;
        }
    }

    if (!adjacency_list_graph_view(a_star_context->graph, &a_star_context->view)) {
        adjacency_list_destroy(a_star_context->graph);
        a_star_context->graph = NULL;
        return false;
    }

    a_star_context->path_length = 0U;
    return true;
}

// Finds the complete list-chain path as one timed operation.
static bool a_star_operation(void *context) {
    struct AStarContext *a_star_context = context;

    return a_star(
        &a_star_context->view,
        a_star_context->nodes[0],
        a_star_context->nodes[ITEM_COUNT - 1U],
        zero_heuristic,
        NULL,
        a_star_context->path,
        ITEM_COUNT,
        &a_star_context->path_length
    );
}

// Confirms A-star returned the complete source-to-goal list-chain path.
static bool verify(void *context) {
    struct AStarContext *a_star_context = context;

    return a_star_context->path_length == ITEM_COUNT &&
        a_star_context->path[0] == 0U &&
        a_star_context->path[ITEM_COUNT - 1U] == ITEM_COUNT - 1U;
}

// Releases the adjacency-list graph created for one benchmark sample.
static void teardown(void *context) {
    struct AStarContext *a_star_context = context;

    adjacency_list_destroy(a_star_context->graph);
    a_star_context->graph = NULL;
}

// Builds a unit-weight directed adjacency-matrix chain outside the timed operation.
static bool matrix_setup(void *context) {
    struct MatrixAStarContext *a_star_context = context;

    a_star_context->graph = adjacency_matrix_create(true);
    if (a_star_context->graph == NULL) {
        return false;
    }

    for (size_t index = 0U; index < MATRIX_ITEM_COUNT; index++) {
        if (!adjacency_matrix_add_node(
                a_star_context->graph,
                &a_star_context->values[index],
                &a_star_context->nodes[index]
            )) {
            adjacency_matrix_destroy(a_star_context->graph);
            a_star_context->graph = NULL;
            return false;
        }
    }

    for (size_t index = 0U; index + 1U < MATRIX_ITEM_COUNT; index++) {
        if (!adjacency_matrix_add_edge(
                a_star_context->graph,
                a_star_context->nodes[index],
                a_star_context->nodes[index + 1U],
                1U
            )) {
            adjacency_matrix_destroy(a_star_context->graph);
            a_star_context->graph = NULL;
            return false;
        }
    }

    if (!adjacency_matrix_graph_view(a_star_context->graph, &a_star_context->view)) {
        adjacency_matrix_destroy(a_star_context->graph);
        a_star_context->graph = NULL;
        return false;
    }

    a_star_context->path_length = 0U;
    return true;
}

// Finds the complete matrix-chain path as one timed operation.
static bool matrix_a_star_operation(void *context) {
    struct MatrixAStarContext *a_star_context = context;

    return a_star(
        &a_star_context->view,
        a_star_context->nodes[0],
        a_star_context->nodes[MATRIX_ITEM_COUNT - 1U],
        zero_heuristic,
        NULL,
        a_star_context->path,
        MATRIX_ITEM_COUNT,
        &a_star_context->path_length
    );
}

// Confirms A-star returned the complete source-to-goal matrix-chain path.
static bool matrix_verify(void *context) {
    struct MatrixAStarContext *a_star_context = context;

    return a_star_context->path_length == MATRIX_ITEM_COUNT &&
        a_star_context->path[0] == 0U &&
        a_star_context->path[MATRIX_ITEM_COUNT - 1U] == MATRIX_ITEM_COUNT - 1U;
}

// Releases the adjacency-matrix graph created for one benchmark sample.
static void matrix_teardown(void *context) {
    struct MatrixAStarContext *a_star_context = context;

    adjacency_matrix_destroy(a_star_context->graph);
    a_star_context->graph = NULL;
}

// Runs and prints one configured A-star benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct AStarContext a_star_context;
    static struct MatrixAStarContext matrix_a_star_context;
    BenchmarkConfig config = {
        .name = "A-star 2000-node adjacency-list chain",
        .warmup_iterations = 1U,
        .sample_count = 21U,
        .iterations_per_sample = 1U,
        .setup = setup,
        .operation = a_star_operation,
        .verify = verify,
        .teardown = teardown,
        .context = &a_star_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        a_star_context.values[index] = (int)index;
    }
    for (size_t index = 0U; index < MATRIX_ITEM_COUNT; index++) {
        matrix_a_star_context.values[index] = (int)index;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "A-star 1000-node adjacency-matrix chain";
    config.context = &matrix_a_star_context;
    config.setup = matrix_setup;
    config.operation = matrix_a_star_operation;
    config.verify = matrix_verify;
    config.teardown = matrix_teardown;
    return run_benchmark(&config) ? 0 : 1;
}
