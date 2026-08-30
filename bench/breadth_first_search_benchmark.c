// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public BFS API.
#include "../src/algorithms/graph-traversal/breadth-first-search/breadth_first_search.h"
// Provides the adjacency-list GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-list/adjacency_list.h"
// Provides the adjacency-matrix GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

// Limits setup and full traversal samples to a practical graph size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 2000U
#endif

// Defines the number of reachable Nodes visited by each timed BFS operation.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };
// Keeps dense matrix setup practical while still traversing a substantial graph.
enum { MATRIX_ITEM_COUNT = 1000U };

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
    // Selects the early-stop target graph instead of the full-traversal chain.
    bool early_exit;
    // Identifies the shallow target Node in an early-stop workload.
    Node *target;
    // Records whether the visitor stopped on the requested target.
    bool found;
};

// Stores state shared by adjacency-matrix BFS benchmark samples.
struct MatrixBreadthFirstSearchContext {
    // Owns the adjacency-matrix graph built before each sample.
    AdjacencyMatrix *graph;
    // Stores caller-owned Node payload values.
    int values[MATRIX_ITEM_COUNT];
    // Stores graph-owned Node handles in traversal order.
    Node *nodes[MATRIX_ITEM_COUNT];
    // Holds the representation-independent graph adapter.
    GraphView view;
    // Counts Nodes visited during the timed BFS operation.
    size_t visited_count;
    // Selects the early-stop target graph instead of the full-traversal chain.
    bool early_exit;
    // Identifies the shallow target Node in an early-stop workload.
    Node *target;
    // Records whether the visitor stopped on the requested target.
    bool found;
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

// Stops immediately when BFS discovers the shallow target Node.
static bool find_list_target(Node *node, void *context) {
    struct BreadthFirstSearchContext *bfs_context = context;

    bfs_context->visited_count++;
    if (node == bfs_context->target) {
        bfs_context->found = true;
        return false;
    }

    return true;
}

// Records each Node visited by matrix-backed BFS without early stopping.
static bool record_matrix_visit(Node *node, void *context) {
    struct MatrixBreadthFirstSearchContext *bfs_context = context;

    if (node == NULL) {
        return false;
    }

    bfs_context->visited_count++;
    return true;
}

// Stops immediately when matrix-backed BFS discovers the shallow target Node.
static bool find_matrix_target(Node *node, void *context) {
    struct MatrixBreadthFirstSearchContext *bfs_context = context;

    bfs_context->visited_count++;
    if (node == bfs_context->target) {
        bfs_context->found = true;
        return false;
    }

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

    if (bfs_context->early_exit) {
        // Enqueues shallow target first, then a deep branch DFS would follow.
        if (!adjacency_list_add_edge(bfs_context->graph, bfs_context->nodes[0], bfs_context->nodes[1], 1U) ||
            !adjacency_list_add_edge(bfs_context->graph, bfs_context->nodes[0], bfs_context->nodes[2], 1U)) {
            adjacency_list_destroy(bfs_context->graph);
            bfs_context->graph = NULL;
            return false;
        }
        for (size_t index = 2U; index + 1U < ITEM_COUNT; index++) {
            if (!adjacency_list_add_edge(bfs_context->graph, bfs_context->nodes[index], bfs_context->nodes[index + 1U], 1U)) {
                adjacency_list_destroy(bfs_context->graph);
                bfs_context->graph = NULL;
                return false;
            }
        }
        bfs_context->target = bfs_context->nodes[1];
    } else {
        for (size_t index = 0U; index + 1U < ITEM_COUNT; index++) {
            if (!adjacency_list_add_edge(bfs_context->graph, bfs_context->nodes[index], bfs_context->nodes[index + 1U], 1U)) {
                adjacency_list_destroy(bfs_context->graph);
                bfs_context->graph = NULL;
                return false;
            }
        }
    }

    if (!adjacency_list_graph_view(bfs_context->graph, &bfs_context->view)) {
        adjacency_list_destroy(bfs_context->graph);
        bfs_context->graph = NULL;
        return false;
    }

    bfs_context->visited_count = 0U;
    bfs_context->found = false;
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

// Confirms the shallow target stopped BFS after source and target visits.
static bool early_verify(void *context) {
    struct BreadthFirstSearchContext *bfs_context = context;

    return bfs_context->found && bfs_context->visited_count == 2U;
}

// Releases the graph created for one benchmark sample.
static void teardown(void *context) {
    struct BreadthFirstSearchContext *bfs_context = context;

    adjacency_list_destroy(bfs_context->graph);
    bfs_context->graph = NULL;
}

// Builds a directed adjacency-matrix chain outside the timed traversal operation.
static bool matrix_setup(void *context) {
    struct MatrixBreadthFirstSearchContext *bfs_context = context;

    bfs_context->graph = adjacency_matrix_create(true);
    if (bfs_context->graph == NULL) {
        return false;
    }

    for (size_t index = 0U; index < MATRIX_ITEM_COUNT; index++) {
        if (!adjacency_matrix_add_node(
                bfs_context->graph,
                &bfs_context->values[index],
                &bfs_context->nodes[index]
            )) {
            adjacency_matrix_destroy(bfs_context->graph);
            bfs_context->graph = NULL;
            return false;
        }
    }

    if (bfs_context->early_exit) {
        if (!adjacency_matrix_add_edge(bfs_context->graph, bfs_context->nodes[0], bfs_context->nodes[1], 1U) ||
            !adjacency_matrix_add_edge(bfs_context->graph, bfs_context->nodes[0], bfs_context->nodes[2], 1U)) {
            adjacency_matrix_destroy(bfs_context->graph);
            bfs_context->graph = NULL;
            return false;
        }
        for (size_t index = 2U; index + 1U < MATRIX_ITEM_COUNT; index++) {
            if (!adjacency_matrix_add_edge(bfs_context->graph, bfs_context->nodes[index], bfs_context->nodes[index + 1U], 1U)) {
                adjacency_matrix_destroy(bfs_context->graph);
                bfs_context->graph = NULL;
                return false;
            }
        }
        bfs_context->target = bfs_context->nodes[1];
    } else {
        for (size_t index = 0U; index + 1U < MATRIX_ITEM_COUNT; index++) {
            if (!adjacency_matrix_add_edge(bfs_context->graph, bfs_context->nodes[index], bfs_context->nodes[index + 1U], 1U)) {
                adjacency_matrix_destroy(bfs_context->graph);
                bfs_context->graph = NULL;
                return false;
            }
        }
    }

    if (!adjacency_matrix_graph_view(bfs_context->graph, &bfs_context->view)) {
        adjacency_matrix_destroy(bfs_context->graph);
        bfs_context->graph = NULL;
        return false;
    }

    bfs_context->visited_count = 0U;
    bfs_context->found = false;
    return true;
}

// Traverses the complete matrix-backed chain as one timed BFS operation.
static bool matrix_breadth_first_search_operation(void *context) {
    struct MatrixBreadthFirstSearchContext *bfs_context = context;

    return breadth_first_search(
        &bfs_context->view,
        bfs_context->nodes[0],
        record_matrix_visit,
        bfs_context
    );
}

// Confirms BFS visited every Node in the reachable matrix chain exactly once.
static bool matrix_verify(void *context) {
    struct MatrixBreadthFirstSearchContext *bfs_context = context;

    return bfs_context->visited_count == MATRIX_ITEM_COUNT;
}

// Confirms the shallow target stopped matrix-backed BFS after two visits.
static bool matrix_early_verify(void *context) {
    struct MatrixBreadthFirstSearchContext *bfs_context = context;

    return bfs_context->found && bfs_context->visited_count == 2U;
}

// Releases the matrix graph created for one benchmark sample.
static void matrix_teardown(void *context) {
    struct MatrixBreadthFirstSearchContext *bfs_context = context;

    adjacency_matrix_destroy(bfs_context->graph);
    bfs_context->graph = NULL;
}

// Traverses the list early-stop graph until the target visitor stops BFS.
static bool list_early_exit_operation(void *context) {
    struct BreadthFirstSearchContext *bfs_context = context;

    return !breadth_first_search(&bfs_context->view, bfs_context->nodes[0], find_list_target, bfs_context) && bfs_context->found;
}

// Traverses the matrix early-stop graph until the target visitor stops BFS.
static bool matrix_early_exit_operation(void *context) {
    struct MatrixBreadthFirstSearchContext *bfs_context = context;

    return !breadth_first_search(&bfs_context->view, bfs_context->nodes[0], find_matrix_target, bfs_context) && bfs_context->found;
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
    static struct MatrixBreadthFirstSearchContext matrix_bfs_context;
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

    if (!run_benchmark(&config)) {
        return 1;
    }

    bfs_context.early_exit = true;
    config.name = "Breadth-first search shallow target early exit (list)";
    config.operation = list_early_exit_operation;
    config.verify = early_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    matrix_bfs_context.early_exit = false;
    config.name = "Breadth-first search 1000-node matrix chain";
    config.context = &matrix_bfs_context;
    config.setup = matrix_setup;
    config.operation = matrix_breadth_first_search_operation;
    config.verify = matrix_verify;
    config.teardown = matrix_teardown;
    if (!run_benchmark(&config)) {
        return 1;
    }

    matrix_bfs_context.early_exit = true;
    config.name = "Breadth-first search shallow target early exit (matrix)";
    config.operation = matrix_early_exit_operation;
    config.verify = matrix_early_verify;
    return run_benchmark(&config) ? 0 : 1;
}
