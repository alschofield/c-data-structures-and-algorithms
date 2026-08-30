// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public BFS API.
#include "../src/algorithms/graph-traversal/breadth-first-search/breadth_first_search.h"
// Provides the adjacency-list GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-list/adjacency_list.h"

// Limits setup and full traversal samples to a practical graph size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 2000U
#endif

// Defines the number of reachable Nodes visited by each timed BFS operation.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state shared by breadth-first-search benchmark samples.
struct BreadthFirstSearchContext {
    // Owns the adjacency-list graph built before each sample.
    AdjacencyList *graph;
    // Stores caller-owned Node payload values.
    int values[ITEM_COUNT];
    // Stores graph-owned Node handles in traversal order.
    Node *nodes[ITEM_COUNT];
    // Holds the representation-independent graph adapter.
    GraphView view;
    // Counts Nodes visited during the timed BFS operation.
    size_t visited_count;
};

// Records each Node visited by BFS without requesting an early stop.
static bool record_visit(Node *node, void *context) {
    struct BreadthFirstSearchContext *bfs_context = context;

    if (node == NULL) {
        return false;
    }

    bfs_context->visited_count++;
    return true;
}

// Builds a directed chain GraphView outside the timed traversal operation.
static bool setup(void *context) {
    struct BreadthFirstSearchContext *bfs_context = context;

    bfs_context->graph = adjacency_list_create(true);
    if (bfs_context->graph == NULL) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!adjacency_list_add_node(
                bfs_context->graph,
                &bfs_context->values[index],
                &bfs_context->nodes[index]
            )) {
            adjacency_list_destroy(bfs_context->graph);
            bfs_context->graph = NULL;
            return false;
        }
    }

    for (size_t index = 0U; index + 1U < ITEM_COUNT; index++) {
        if (!adjacency_list_add_edge(
                bfs_context->graph,
                bfs_context->nodes[index],
                bfs_context->nodes[index + 1U],
                1U
            )) {
            adjacency_list_destroy(bfs_context->graph);
            bfs_context->graph = NULL;
            return false;
        }
    }

    if (!adjacency_list_graph_view(bfs_context->graph, &bfs_context->view)) {
        adjacency_list_destroy(bfs_context->graph);
        bfs_context->graph = NULL;
        return false;
    }

    bfs_context->visited_count = 0U;
    return true;
}

// Traverses the complete reachable chain as one timed BFS operation.
static bool breadth_first_search_operation(void *context) {
    struct BreadthFirstSearchContext *bfs_context = context;

    return breadth_first_search(
        &bfs_context->view,
        bfs_context->nodes[0],
        record_visit,
        bfs_context
    );
}

// Confirms BFS visited every Node in the reachable chain exactly once.
static bool verify(void *context) {
    struct BreadthFirstSearchContext *bfs_context = context;

    return bfs_context->visited_count == ITEM_COUNT;
}

// Releases the graph created for one benchmark sample.
static void teardown(void *context) {
    struct BreadthFirstSearchContext *bfs_context = context;

    adjacency_list_destroy(bfs_context->graph);
    bfs_context->graph = NULL;
}

// Runs and prints one configured BFS benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct BreadthFirstSearchContext bfs_context;
    BenchmarkConfig config = {
        .name = "Breadth-first search 2000-node chain",
        .warmup_iterations = 1U,
        .sample_count = 21U,
        .iterations_per_sample = 1U,
        .setup = setup,
        .operation = breadth_first_search_operation,
        .verify = verify,
        .teardown = teardown,
        .context = &bfs_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        bfs_context.values[index] = (int)index;
    }

    return run_benchmark(&config) ? 0 : 1;
}
