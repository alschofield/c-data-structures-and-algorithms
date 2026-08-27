// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public adjacency-list API.
#include "../src/data-structures/graphs/representations/adjacency-list/adjacency_list.h"

// Uses a substantial graph size unless Make supplies an experiment size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 2000U
#endif

// Defines the number of distinct target nodes used in every sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state shared by adjacency-list operation benchmarks.
struct AdjacencyListContext {
    // Owns the graph created before each benchmark sample.
    AdjacencyList *graph;
    // Stores caller-owned node payloads.
    int values[ITEM_COUNT + 1U];
    // Stores the source node for every directed edge operation.
    Node *source;
    // Stores every distinct target node.
    Node *targets[ITEM_COUNT];
    // Tracks the next target used by a timed operation.
    size_t next_index;
};

// Creates a directed graph with a source node and distinct target nodes.
static bool graph_setup(void *context) {
    struct AdjacencyListContext *list_context = context;

    list_context->graph = adjacency_list_create(true);
    if (list_context->graph == NULL) {
        return false;
    }

    if (!adjacency_list_add_node(
            list_context->graph,
            &list_context->values[0],
            &list_context->source
        )) {
        adjacency_list_destroy(list_context->graph);
        list_context->graph = NULL;
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!adjacency_list_add_node(
                list_context->graph,
                &list_context->values[index + 1U],
                &list_context->targets[index]
            )) {
            adjacency_list_destroy(list_context->graph);
            list_context->graph = NULL;
            return false;
        }
    }

    list_context->next_index = 0U;
    return true;
}

// Populates every source-to-target edge outside timed update and lookup samples.
static bool populated_setup(void *context) {
    struct AdjacencyListContext *list_context = context;

    if (!graph_setup(context)) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!adjacency_list_add_edge(
                list_context->graph,
                list_context->source,
                list_context->targets[index],
                1U
            )) {
            adjacency_list_destroy(list_context->graph);
            list_context->graph = NULL;
            return false;
        }
    }

    list_context->next_index = 0U;
    return true;
}

// Inserts one previously absent directed edge.
static bool insert_operation(void *context) {
    struct AdjacencyListContext *list_context = context;

    if (list_context->next_index == ITEM_COUNT || !adjacency_list_add_edge(
            list_context->graph,
            list_context->source,
            list_context->targets[list_context->next_index],
            1U
        )) {
        return false;
    }

    list_context->next_index++;
    return true;
}

// Updates one existing edge's weight without adding a logical edge.
static bool update_operation(void *context) {
    struct AdjacencyListContext *list_context = context;

    if (list_context->next_index == ITEM_COUNT || !adjacency_list_add_edge(
            list_context->graph,
            list_context->source,
            list_context->targets[list_context->next_index],
            (uint64_t)(list_context->next_index + 2U)
        )) {
        return false;
    }

    list_context->next_index++;
    return true;
}

// Looks up one existing source-to-target edge.
static bool has_edge_operation(void *context) {
    struct AdjacencyListContext *list_context = context;

    if (list_context->next_index == ITEM_COUNT || !adjacency_list_has_edge(
            list_context->graph,
            list_context->source,
            list_context->targets[list_context->next_index]
        )) {
        return false;
    }

    list_context->next_index++;
    return true;
}

// Confirms every timed insertion created one logical directed edge.
static bool insert_verify(void *context) {
    struct AdjacencyListContext *list_context = context;

    return list_context->next_index == ITEM_COUNT &&
        adjacency_list_edge_count(list_context->graph) == ITEM_COUNT;
}

// Confirms updates and lookups preserved all populated logical edges.
static bool populated_verify(void *context) {
    struct AdjacencyListContext *list_context = context;

    return list_context->next_index == ITEM_COUNT &&
        adjacency_list_edge_count(list_context->graph) == ITEM_COUNT;
}

// Releases the graph created for one benchmark sample.
static void graph_teardown(void *context) {
    struct AdjacencyListContext *list_context = context;

    adjacency_list_destroy(list_context->graph);
    list_context->graph = NULL;
}

// Runs and prints one configured adjacency-list benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct AdjacencyListContext list_context;
    BenchmarkConfig config = {
        .name = "Adjacency list insert edge",
        .warmup_iterations = ITEM_COUNT,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = graph_setup,
        .operation = insert_operation,
        .verify = insert_verify,
        .teardown = graph_teardown,
        .context = &list_context
    };

    for (size_t index = 0U; index <= ITEM_COUNT; index++) {
        list_context.values[index] = (int)index;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Adjacency list update edge";
    config.setup = populated_setup;
    config.operation = update_operation;
    config.verify = populated_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Adjacency list has edge";
    config.operation = has_edge_operation;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
