// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public DFS API.
#include "../src/algorithms/graph-traversal/depth-first-search/depth_first_search.h"
// Provides the adjacency-list GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-list/adjacency_list.h"
// Provides the adjacency-matrix GraphView adapter used by this benchmark.
#include "../src/data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

// Limits adjacency-list setup and full traversal samples to a practical size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 2000U
#endif

// Defines the number of reachable Nodes visited by list-backed DFS samples.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };
// Keeps dense matrix setup practical while still traversing a substantial graph.
enum { MATRIX_ITEM_COUNT = 1000U };

// Stores state shared by adjacency-list DFS benchmark samples.
struct DepthFirstSearchContext {
    AdjacencyList *graph;
    int values[ITEM_COUNT];
    Node *nodes[ITEM_COUNT];
    GraphView view;
    size_t visited_count;
    bool early_exit;
    Node *target;
    bool found;
};

// Stores state shared by adjacency-matrix DFS benchmark samples.
struct MatrixDepthFirstSearchContext {
    AdjacencyMatrix *graph;
    int values[MATRIX_ITEM_COUNT];
    Node *nodes[MATRIX_ITEM_COUNT];
    GraphView view;
    size_t visited_count;
    bool early_exit;
    Node *target;
    bool found;
};

// Records one visited Node without requesting an early stop.
static bool record_visit(Node *node, void *context) {
    struct DepthFirstSearchContext *dfs_context = context;

    if (node == NULL) {
        return false;
    }

    dfs_context->visited_count++;
    return true;
}

// Stops when DFS reaches the deep branch target.
static bool find_list_target(Node *node, void *context) {
    struct DepthFirstSearchContext *dfs_context = context;

    dfs_context->visited_count++;
    if (node == dfs_context->target) {
        dfs_context->found = true;
        return false;
    }

    return true;
}

// Records one matrix-backed visited Node without requesting an early stop.
static bool record_matrix_visit(Node *node, void *context) {
    struct MatrixDepthFirstSearchContext *dfs_context = context;

    if (node == NULL) {
        return false;
    }

    dfs_context->visited_count++;
    return true;
}

// Stops when matrix-backed DFS reaches the deep branch target.
static bool find_matrix_target(Node *node, void *context) {
    struct MatrixDepthFirstSearchContext *dfs_context = context;

    dfs_context->visited_count++;
    if (node == dfs_context->target) {
        dfs_context->found = true;
        return false;
    }

    return true;
}

// Builds a directed adjacency-list chain outside the timed traversal operation.
static bool setup(void *context) {
    struct DepthFirstSearchContext *dfs_context = context;

    dfs_context->graph = adjacency_list_create(true);
    if (dfs_context->graph == NULL) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!adjacency_list_add_node(dfs_context->graph, &dfs_context->values[index], &dfs_context->nodes[index])) {
            adjacency_list_destroy(dfs_context->graph);
            dfs_context->graph = NULL;
            return false;
        }
    }

    if (dfs_context->early_exit) {
        // The last-enumerated deep branch is popped before the first wide branch.
        if (!adjacency_list_add_edge(dfs_context->graph, dfs_context->nodes[0], dfs_context->nodes[1], 1U) ||
            !adjacency_list_add_edge(dfs_context->graph, dfs_context->nodes[0], dfs_context->nodes[2], 1U) ||
            !adjacency_list_add_edge(dfs_context->graph, dfs_context->nodes[2], dfs_context->nodes[3], 1U)) {
            adjacency_list_destroy(dfs_context->graph);
            dfs_context->graph = NULL;
            return false;
        }
        for (size_t index = 4U; index < ITEM_COUNT; index++) {
            if (!adjacency_list_add_edge(dfs_context->graph, dfs_context->nodes[1], dfs_context->nodes[index], 1U)) {
                adjacency_list_destroy(dfs_context->graph);
                dfs_context->graph = NULL;
                return false;
            }
        }
        dfs_context->target = dfs_context->nodes[3];
    } else {
        for (size_t index = 0U; index + 1U < ITEM_COUNT; index++) {
            if (!adjacency_list_add_edge(dfs_context->graph, dfs_context->nodes[index], dfs_context->nodes[index + 1U], 1U)) {
                adjacency_list_destroy(dfs_context->graph);
                dfs_context->graph = NULL;
                return false;
            }
        }
    }

    if (!adjacency_list_graph_view(dfs_context->graph, &dfs_context->view)) {
        adjacency_list_destroy(dfs_context->graph);
        dfs_context->graph = NULL;
        return false;
    }

    dfs_context->visited_count = 0U;
    dfs_context->found = false;
    return true;
}

// Traverses the complete list-backed chain as one timed DFS operation.
static bool depth_first_search_operation(void *context) {
    struct DepthFirstSearchContext *dfs_context = context;

    return depth_first_search(&dfs_context->view, dfs_context->nodes[0], record_visit, dfs_context);
}

// Confirms DFS visited every Node in the reachable list chain exactly once.
static bool verify(void *context) {
    struct DepthFirstSearchContext *dfs_context = context;

    return dfs_context->visited_count == ITEM_COUNT;
}

// Confirms the deep target stopped list-backed DFS after three visits.
static bool early_verify(void *context) {
    struct DepthFirstSearchContext *dfs_context = context;

    return dfs_context->found && dfs_context->visited_count == 3U;
}

// Releases the adjacency-list graph created for one benchmark sample.
static void teardown(void *context) {
    struct DepthFirstSearchContext *dfs_context = context;

    adjacency_list_destroy(dfs_context->graph);
    dfs_context->graph = NULL;
}

// Builds a directed adjacency-matrix chain outside the timed traversal operation.
static bool matrix_setup(void *context) {
    struct MatrixDepthFirstSearchContext *dfs_context = context;

    dfs_context->graph = adjacency_matrix_create(true);
    if (dfs_context->graph == NULL) {
        return false;
    }

    for (size_t index = 0U; index < MATRIX_ITEM_COUNT; index++) {
        if (!adjacency_matrix_add_node(dfs_context->graph, &dfs_context->values[index], &dfs_context->nodes[index])) {
            adjacency_matrix_destroy(dfs_context->graph);
            dfs_context->graph = NULL;
            return false;
        }
    }

    if (dfs_context->early_exit) {
        if (!adjacency_matrix_add_edge(dfs_context->graph, dfs_context->nodes[0], dfs_context->nodes[1], 1U) ||
            !adjacency_matrix_add_edge(dfs_context->graph, dfs_context->nodes[0], dfs_context->nodes[2], 1U) ||
            !adjacency_matrix_add_edge(dfs_context->graph, dfs_context->nodes[2], dfs_context->nodes[3], 1U)) {
            adjacency_matrix_destroy(dfs_context->graph);
            dfs_context->graph = NULL;
            return false;
        }
        for (size_t index = 4U; index < MATRIX_ITEM_COUNT; index++) {
            if (!adjacency_matrix_add_edge(dfs_context->graph, dfs_context->nodes[1], dfs_context->nodes[index], 1U)) {
                adjacency_matrix_destroy(dfs_context->graph);
                dfs_context->graph = NULL;
                return false;
            }
        }
        dfs_context->target = dfs_context->nodes[3];
    } else {
        for (size_t index = 0U; index + 1U < MATRIX_ITEM_COUNT; index++) {
            if (!adjacency_matrix_add_edge(dfs_context->graph, dfs_context->nodes[index], dfs_context->nodes[index + 1U], 1U)) {
                adjacency_matrix_destroy(dfs_context->graph);
                dfs_context->graph = NULL;
                return false;
            }
        }
    }

    if (!adjacency_matrix_graph_view(dfs_context->graph, &dfs_context->view)) {
        adjacency_matrix_destroy(dfs_context->graph);
        dfs_context->graph = NULL;
        return false;
    }

    dfs_context->visited_count = 0U;
    dfs_context->found = false;
    return true;
}

// Traverses the complete matrix-backed chain as one timed DFS operation.
static bool matrix_depth_first_search_operation(void *context) {
    struct MatrixDepthFirstSearchContext *dfs_context = context;

    return depth_first_search(&dfs_context->view, dfs_context->nodes[0], record_matrix_visit, dfs_context);
}

// Confirms DFS visited every Node in the reachable matrix chain exactly once.
static bool matrix_verify(void *context) {
    struct MatrixDepthFirstSearchContext *dfs_context = context;

    return dfs_context->visited_count == MATRIX_ITEM_COUNT;
}

// Confirms the deep target stopped matrix-backed DFS after three visits.
static bool matrix_early_verify(void *context) {
    struct MatrixDepthFirstSearchContext *dfs_context = context;

    return dfs_context->found && dfs_context->visited_count == 3U;
}

// Releases the adjacency-matrix graph created for one benchmark sample.
static void matrix_teardown(void *context) {
    struct MatrixDepthFirstSearchContext *dfs_context = context;

    adjacency_matrix_destroy(dfs_context->graph);
    dfs_context->graph = NULL;
}

// Traverses the list early-stop graph until the target visitor stops DFS.
static bool list_early_exit_operation(void *context) {
    struct DepthFirstSearchContext *dfs_context = context;

    return !depth_first_search(&dfs_context->view, dfs_context->nodes[0], find_list_target, dfs_context) && dfs_context->found;
}

// Traverses the matrix early-stop graph until the target visitor stops DFS.
static bool matrix_early_exit_operation(void *context) {
    struct MatrixDepthFirstSearchContext *dfs_context = context;

    return !depth_first_search(&dfs_context->view, dfs_context->nodes[0], find_matrix_target, dfs_context) && dfs_context->found;
}

// Runs and prints one configured DFS benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct DepthFirstSearchContext dfs_context;
    static struct MatrixDepthFirstSearchContext matrix_dfs_context;
    BenchmarkConfig config = {
        .name = "Depth-first search 2000-node chain",
        .warmup_iterations = 1U,
        .sample_count = 21U,
        .iterations_per_sample = 1U,
        .setup = setup,
        .operation = depth_first_search_operation,
        .verify = verify,
        .teardown = teardown,
        .context = &dfs_context
    };

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        dfs_context.values[index] = (int)index;
    }
    for (size_t index = 0U; index < MATRIX_ITEM_COUNT; index++) {
        matrix_dfs_context.values[index] = (int)index;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    dfs_context.early_exit = true;
    config.name = "Depth-first search deep target early exit (list)";
    config.operation = list_early_exit_operation;
    config.verify = early_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    matrix_dfs_context.early_exit = false;
    config.name = "Depth-first search 1000-node matrix chain";
    config.context = &matrix_dfs_context;
    config.setup = matrix_setup;
    config.operation = matrix_depth_first_search_operation;
    config.verify = matrix_verify;
    config.teardown = matrix_teardown;
    if (!run_benchmark(&config)) {
        return 1;
    }

    matrix_dfs_context.early_exit = true;
    config.name = "Depth-first search deep target early exit (matrix)";
    config.operation = matrix_early_exit_operation;
    config.verify = matrix_early_verify;
    return run_benchmark(&config) ? 0 : 1;
}
