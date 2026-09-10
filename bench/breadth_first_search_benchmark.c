#include "benchmark.h"
#include "../src/algorithms/graph-traversal/breadth-first-search/breadth_first_search.h"
#include "../src/data-structures/graphs/representations/adjacency-list/adjacency_list.h"
#include "../src/data-structures/graphs/representations/adjacency-matrix/adjacency_matrix.h"

#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 2000U
#endif

enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };
enum { MATRIX_ITEM_COUNT = 1000U };

struct ListContext { AdjacencyList *graph; int values[ITEM_COUNT]; Node *nodes[ITEM_COUNT]; GraphView view; size_t visited; };
struct MatrixContext { AdjacencyMatrix *graph; int values[MATRIX_ITEM_COUNT]; Node *nodes[MATRIX_ITEM_COUNT]; GraphView view; size_t visited; };

static bool visit_list(size_t index, void *context) { (void)index; ((struct ListContext *)context)->visited++; return true; }
static bool visit_matrix(size_t index, void *context) { (void)index; ((struct MatrixContext *)context)->visited++; return true; }

static bool list_setup(void *context) {
    struct ListContext *state = context;
    state->graph = adjacency_list_create(true);
    if (state->graph == NULL) return false;
    for (size_t i = 0U; i < ITEM_COUNT; i++) if (!adjacency_list_add_node(state->graph, &state->values[i], &state->nodes[i])) return false;
    for (size_t i = 0U; i + 1U < ITEM_COUNT; i++) if (!adjacency_list_add_edge(state->graph, state->nodes[i], state->nodes[i + 1U], 1U)) return false;
    if (!adjacency_list_graph_view(state->graph, &state->view)) return false;
    state->visited = 0U;
    return true;
}
static bool matrix_setup(void *context) {
    struct MatrixContext *state = context;
    state->graph = adjacency_matrix_create(true);
    if (state->graph == NULL) return false;
    for (size_t i = 0U; i < MATRIX_ITEM_COUNT; i++) if (!adjacency_matrix_add_node(state->graph, &state->values[i], &state->nodes[i])) return false;
    for (size_t i = 0U; i + 1U < MATRIX_ITEM_COUNT; i++) if (!adjacency_matrix_add_edge(state->graph, state->nodes[i], state->nodes[i + 1U], 1U)) return false;
    if (!adjacency_matrix_graph_view(state->graph, &state->view)) return false;
    state->visited = 0U;
    return true;
}
static bool list_operation(void *context) { struct ListContext *state = context; return breadth_first_search(&state->view, 0U, visit_list, state); }
static bool matrix_operation(void *context) { struct MatrixContext *state = context; return breadth_first_search(&state->view, 0U, visit_matrix, state); }
static bool list_verify(void *context) { return ((struct ListContext *)context)->visited == ITEM_COUNT; }
static bool matrix_verify(void *context) { return ((struct MatrixContext *)context)->visited == MATRIX_ITEM_COUNT; }
static void list_teardown(void *context) { struct ListContext *state = context; adjacency_list_destroy(state->graph); state->graph = NULL; }
static void matrix_teardown(void *context) { struct MatrixContext *state = context; adjacency_matrix_destroy(state->graph); state->graph = NULL; }
static bool run(const BenchmarkConfig *config) { BenchmarkResult result; if (!benchmark_run(config, &result)) return false; benchmark_print_result(&result); return true; }

int main(void) {
    static struct ListContext list;
    static struct MatrixContext matrix;
    for (size_t i = 0U; i < ITEM_COUNT; i++) list.values[i] = (int)i;
    for (size_t i = 0U; i < MATRIX_ITEM_COUNT; i++) matrix.values[i] = (int)i;
    BenchmarkConfig config = { .name = "Breadth-first search 2000-node list chain", .warmup_iterations = 1U, .sample_count = 21U, .iterations_per_sample = 1U, .setup = list_setup, .operation = list_operation, .verify = list_verify, .teardown = list_teardown, .context = &list };
    if (!run(&config)) return 1;
    config.name = "Breadth-first search 1000-node matrix chain";
    config.setup = matrix_setup; config.operation = matrix_operation; config.verify = matrix_verify; config.teardown = matrix_teardown; config.context = &matrix;
    return run(&config) ? 0 : 1;
}
