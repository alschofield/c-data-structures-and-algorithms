// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public Kruskal API.
#include "../src/algorithms/minimum-spanning-trees/kruskal/kruskal.h"
// Provides the adjacency-list GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-list/adjacency_list.h"
// Provides the adjacency-matrix GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

// Limits sparse graph setup and complete forest samples to a practical size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 2000U
#endif

// Defines the number of Nodes in list-backed benchmark forests.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };
// Keeps dense matrix setup practical while still measuring a substantial forest.
enum { MATRIX_ITEM_COUNT = 1000U };

// Stores state shared by adjacency-list Kruskal benchmark samples.
struct KruskalContext {
    // Owns the graph built outside each timed sample.
    AdjacencyList *graph;
    // Stores caller-owned Node payload values.
    int values[ITEM_COUNT];
    // Stores graph-owned Node handles in chain order.
    Node *nodes[ITEM_COUNT];
    // Holds the representation-independent graph adapter.
    GraphView view;
    // Receives the selected minimum-spanning-forest edges.
    KruskalEdge edges[ITEM_COUNT - 1U];
    // Receives the number of selected forest edges.
    size_t edge_count;
    // Receives the total selected forest weight.
    uint64_t total_weight;
};

// Stores state shared by adjacency-matrix Kruskal benchmark samples.
struct MatrixKruskalContext {
    // Owns the graph built outside each timed sample.
    AdjacencyMatrix *graph;
    // Stores caller-owned Node payload values.
    int values[MATRIX_ITEM_COUNT];
    // Stores graph-owned Node handles in chain order.
    Node *nodes[MATRIX_ITEM_COUNT];
    // Holds the representation-independent graph adapter.
    GraphView view;
    // Receives the selected minimum-spanning-forest edges.
    KruskalEdge edges[MATRIX_ITEM_COUNT - 1U];
    // Receives the number of selected forest edges.
    size_t edge_count;
    // Receives the total selected forest weight.
    uint64_t total_weight;
};

// Returns the sum of distinct chain weights from one through item_count minus one.
static uint64_t expected_total_weight(size_t item_count) {
    return (uint64_t)item_count * (item_count - 1U) / 2U;
}

// Builds a descending-weight undirected adjacency-list chain outside timed work.
static bool setup(void *context) {
    struct KruskalContext *kruskal_context = context;

    kruskal_context->graph = adjacency_list_create(false);
    if (kruskal_context->graph == NULL) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!adjacency_list_add_node(
                kruskal_context->graph,
                &kruskal_context->values[index],
                &kruskal_context->nodes[index]
            )) {
            adjacency_list_destroy(kruskal_context->graph);
            kruskal_context->graph = NULL;
            return false;
        }
    }

    for (size_t index = 0U; index + 1U < ITEM_COUNT; index++) {
        if (!adjacency_list_add_edge(
                kruskal_context->graph,
                kruskal_context->nodes[index],
                kruskal_context->nodes[index + 1U],
                ITEM_COUNT - index - 1U
            )) {
            adjacency_list_destroy(kruskal_context->graph);
            kruskal_context->graph = NULL;
            return false;
        }
    }

    if (!adjacency_list_graph_view(kruskal_context->graph, &kruskal_context->view)) {
        adjacency_list_destroy(kruskal_context->graph);
        kruskal_context->graph = NULL;
        return false;
    }

    kruskal_context->edge_count = 0U;
    kruskal_context->total_weight = 0U;
    return true;
}

// Selects the complete list-chain forest as one timed operation.
static bool kruskal_operation(void *context) {
    struct KruskalContext *kruskal_context = context;

    return kruskal_minimum_spanning_forest(
        &kruskal_context->view,
        kruskal_context->edges,
        ITEM_COUNT - 1U,
        &kruskal_context->edge_count,
        &kruskal_context->total_weight
    );
}

// Confirms Kruskal selected every mandatory chain edge with the expected total.
static bool verify(void *context) {
    struct KruskalContext *kruskal_context = context;

    return kruskal_context->edge_count == ITEM_COUNT - 1U &&
        kruskal_context->total_weight == expected_total_weight(ITEM_COUNT);
}

// Releases the adjacency-list graph created for one benchmark sample.
static void teardown(void *context) {
    struct KruskalContext *kruskal_context = context;

    adjacency_list_destroy(kruskal_context->graph);
    kruskal_context->graph = NULL;
}

// Builds a descending-weight undirected adjacency-matrix chain outside timed work.
static bool matrix_setup(void *context) {
    struct MatrixKruskalContext *kruskal_context = context;

    kruskal_context->graph = adjacency_matrix_create(false);
    if (kruskal_context->graph == NULL) {
        return false;
    }

    for (size_t index = 0U; index < MATRIX_ITEM_COUNT; index++) {
        if (!adjacency_matrix_add_node(
                kruskal_context->graph,
                &kruskal_context->values[index],
                &kruskal_context->nodes[index]
            )) {
            adjacency_matrix_destroy(kruskal_context->graph);
            kruskal_context->graph = NULL;
            return false;
        }
    }

    for (size_t index = 0U; index + 1U < MATRIX_ITEM_COUNT; index++) {
        if (!adjacency_matrix_add_edge(
                kruskal_context->graph,
                kruskal_context->nodes[index],
                kruskal_context->nodes[index + 1U],
                MATRIX_ITEM_COUNT - index - 1U
            )) {
            adjacency_matrix_destroy(kruskal_context->graph);
            kruskal_context->graph = NULL;
            return false;
        }
    }

    if (!adjacency_matrix_graph_view(kruskal_context->graph, &kruskal_context->view)) {
        adjacency_matrix_destroy(kruskal_context->graph);
        kruskal_context->graph = NULL;
        return false;
    }

    kruskal_context->edge_count = 0U;
    kruskal_context->total_weight = 0U;
    return true;
}

// Selects the complete matrix-chain forest as one timed operation.
static bool matrix_kruskal_operation(void *context) {
    struct MatrixKruskalContext *kruskal_context = context;

    return kruskal_minimum_spanning_forest(
        &kruskal_context->view,
        kruskal_context->edges,
        MATRIX_ITEM_COUNT - 1U,
        &kruskal_context->edge_count,
        &kruskal_context->total_weight
    );
}

// Confirms Kruskal selected every mandatory matrix-chain edge with expected total.
static bool matrix_verify(void *context) {
    struct MatrixKruskalContext *kruskal_context = context;

    return kruskal_context->edge_count == MATRIX_ITEM_COUNT - 1U &&
        kruskal_context->total_weight == expected_total_weight(MATRIX_ITEM_COUNT);
}

// Releases the adjacency-matrix graph created for one benchmark sample.
static void matrix_teardown(void *context) {
    struct MatrixKruskalContext *kruskal_context = context;

    adjacency_matrix_destroy(kruskal_context->graph);
    kruskal_context->graph = NULL;
}

// Runs and prints one configured Kruskal benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct KruskalContext kruskal_context;
    static struct MatrixKruskalContext matrix_kruskal_context;
    BenchmarkConfig config = {
        .name = "Kruskal 2000-node adjacency-list chain",
        .warmup_iterations = 1U,
        .sample_count = 21U,
        .iterations_per_sample = 1U,
        .setup = setup,
        .operation = kruskal_operation,
        .verify = verify,
        .teardown = teardown,
        .context = &kruskal_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        kruskal_context.values[index] = (int)index;
    }
    for (size_t index = 0U; index < MATRIX_ITEM_COUNT; index++) {
        matrix_kruskal_context.values[index] = (int)index;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Kruskal 1000-node adjacency-matrix chain";
    config.context = &matrix_kruskal_context;
    config.setup = matrix_setup;
    config.operation = matrix_kruskal_operation;
    config.verify = matrix_verify;
    config.teardown = matrix_teardown;
    return run_benchmark(&config) ? 0 : 1;
}
