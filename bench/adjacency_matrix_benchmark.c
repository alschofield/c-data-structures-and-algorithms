// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public adjacency-matrix API.
#include "../src/data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

// Limits the default dense matrix to a practical benchmark allocation.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 1000U
#endif

// Defines the number of distinct target nodes used in every sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state shared by adjacency-matrix operation benchmarks.
struct AdjacencyMatrixContext {
    // Owns the graph created before each benchmark sample.
    AdjacencyMatrix *graph;
    // Stores caller-owned node payloads.
    int values[ITEM_COUNT + 1U];
    // Stores the source node for every directed edge operation.
    Node *source;
    // Stores every distinct target node.
    Node *targets[ITEM_COUNT];
    // Tracks the next target used by a timed operation.
    size_t next_index;
};

// Creates a directed graph with one source and distinct target nodes.
static bool graph_setup(void *context) {
    struct AdjacencyMatrixContext *matrix_context = context;

    matrix_context->graph = adjacency_matrix_create(true);
    if (matrix_context->graph == NULL) {
        return false;
    }

    if (!adjacency_matrix_add_node(
            matrix_context->graph,
            &matrix_context->values[0],
            &matrix_context->source
        )) {
        adjacency_matrix_destroy(matrix_context->graph);
        matrix_context->graph = NULL;
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!adjacency_matrix_add_node(
                matrix_context->graph,
                &matrix_context->values[index + 1U],
                &matrix_context->targets[index]
            )) {
            adjacency_matrix_destroy(matrix_context->graph);
            matrix_context->graph = NULL;
            return false;
        }
    }

    matrix_context->next_index = 0U;
    return true;
}

// Populates source-to-target edges outside timed lookup and removal samples.
static bool populated_setup(void *context) {
    struct AdjacencyMatrixContext *matrix_context = context;

    if (!graph_setup(context)) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!adjacency_matrix_add_edge(
                matrix_context->graph,
                matrix_context->source,
                matrix_context->targets[index],
                1U
            )) {
            adjacency_matrix_destroy(matrix_context->graph);
            matrix_context->graph = NULL;
            return false;
        }
    }

    matrix_context->next_index = 0U;
    return true;
}

// Inserts one previously absent directed edge.
static bool insert_operation(void *context) {
    struct AdjacencyMatrixContext *matrix_context = context;

    if (matrix_context->next_index == ITEM_COUNT || !adjacency_matrix_add_edge(
            matrix_context->graph,
            matrix_context->source,
            matrix_context->targets[matrix_context->next_index],
            1U
        )) {
        return false;
    }

    matrix_context->next_index++;
    return true;
}

// Looks up one existing source-to-target edge.
static bool has_edge_operation(void *context) {
    struct AdjacencyMatrixContext *matrix_context = context;

    if (matrix_context->next_index == ITEM_COUNT || !adjacency_matrix_has_edge(
            matrix_context->graph,
            matrix_context->source,
            matrix_context->targets[matrix_context->next_index]
        )) {
        return false;
    }

    matrix_context->next_index++;
    return true;
}

// Removes one existing source-to-target edge.
static bool remove_operation(void *context) {
    struct AdjacencyMatrixContext *matrix_context = context;

    if (matrix_context->next_index == ITEM_COUNT || !adjacency_matrix_remove_edge(
            matrix_context->graph,
            matrix_context->source,
            matrix_context->targets[matrix_context->next_index]
        )) {
        return false;
    }

    matrix_context->next_index++;
    return true;
}

// Confirms every timed insertion created one logical directed edge.
static bool insert_verify(void *context) {
    struct AdjacencyMatrixContext *matrix_context = context;

    return matrix_context->next_index == ITEM_COUNT &&
        adjacency_matrix_edge_count(matrix_context->graph) == ITEM_COUNT;
}

// Confirms every timed lookup preserved populated edges.
static bool lookup_verify(void *context) {
    struct AdjacencyMatrixContext *matrix_context = context;

    return matrix_context->next_index == ITEM_COUNT &&
        adjacency_matrix_edge_count(matrix_context->graph) == ITEM_COUNT;
}

// Confirms every timed removal cleared one logical directed edge.
static bool remove_verify(void *context) {
    struct AdjacencyMatrixContext *matrix_context = context;

    return matrix_context->next_index == ITEM_COUNT &&
        adjacency_matrix_edge_count(matrix_context->graph) == 0U;
}

// Releases the graph created for one benchmark sample.
static void graph_teardown(void *context) {
    struct AdjacencyMatrixContext *matrix_context = context;

    adjacency_matrix_destroy(matrix_context->graph);
    matrix_context->graph = NULL;
}

// Runs and prints one configured adjacency-matrix benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct AdjacencyMatrixContext matrix_context;
    BenchmarkConfig config = {
        .name = "Adjacency matrix insert edge",
        .warmup_iterations = ITEM_COUNT,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = graph_setup,
        .operation = insert_operation,
        .verify = insert_verify,
        .teardown = graph_teardown,
        .context = &matrix_context
    };

    for (size_t index = 0U; index <= ITEM_COUNT; index++) {
        matrix_context.values[index] = (int)index;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Adjacency matrix has edge";
    config.setup = populated_setup;
    config.operation = has_edge_operation;
    config.verify = lookup_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Adjacency matrix remove edge";
    config.operation = remove_operation;
    config.verify = remove_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
