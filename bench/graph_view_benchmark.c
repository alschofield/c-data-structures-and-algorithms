// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public GraphView API.
#include "../src/data-structures/graphs/graph-view/graph_view.h"

#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

struct GraphViewContext {
    GraphView view;
    size_t next_index;
    size_t visited_count;
    size_t vertex_count;
};

static size_t vertex_count(const void *context) {
    (void)context;
    return ITEM_COUNT;
}

static bool is_directed(const void *context) {
    (void)context;
    return true;
}

static bool node_at(const void *context, size_t index) {
    (void)context;
    return index < ITEM_COUNT;
}

static bool neighbors(const void *context, size_t index,
                      GraphViewVisitFn visit, void *visit_context) {
    (void)context;
    return visit((index + 1U) % ITEM_COUNT, 1U, visit_context);
}

static bool record_neighbor(size_t neighbor_index, uint64_t weight,
                            void *context) {
    (void)neighbor_index;
    if (weight != 1U) return false;
    ((struct GraphViewContext *)context)->visited_count++;
    return true;
}

static bool setup(void *context) {
    struct GraphViewContext *view_context = context;
    view_context->next_index = 0U;
    view_context->visited_count = 0U;
    view_context->vertex_count = 0U;
    return true;
}

static bool vertex_count_operation(void *context) {
    struct GraphViewContext *view_context = context;
    view_context->vertex_count = graph_view_vertex_count(&view_context->view);
    return view_context->vertex_count == ITEM_COUNT;
}

static bool node_at_operation(void *context) {
    struct GraphViewContext *view_context = context;
    if (!graph_view_node_at(&view_context->view, view_context->next_index)) return false;
    view_context->next_index++;
    return true;
}

static bool neighbors_operation(void *context) {
    struct GraphViewContext *view_context = context;
    if (!graph_view_neighbors(&view_context->view, view_context->next_index,
                              record_neighbor, view_context)) return false;
    view_context->next_index++;
    return true;
}

static bool vertex_count_verify(void *context) {
    return ((struct GraphViewContext *)context)->vertex_count == ITEM_COUNT;
}

static bool node_at_verify(void *context) {
    size_t next_index = ((struct GraphViewContext *)context)->next_index;

    return next_index > 0U && next_index <= ITEM_COUNT;
}

static bool neighbors_verify(void *context) {
    struct GraphViewContext *view_context = context;
    return view_context->next_index > 0U &&
        view_context->next_index <= ITEM_COUNT &&
        view_context->visited_count == view_context->next_index;
}

static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;
    if (!benchmark_run(config, &result)) return false;
    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct GraphViewContext context;
    context.view = (GraphView) {
        .context = &context,
        .vertex_count = vertex_count,
        .is_directed = is_directed,
        .node_at = node_at,
        .neighbors = neighbors
    };
    BenchmarkConfig config = {
        .name = "GraphView vertex count",
        .warmup_iterations = 1U,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = setup,
        .operation = vertex_count_operation,
        .verify = vertex_count_verify,
        .teardown = NULL,
        .context = &context
    };
    if (!run_benchmark(&config)) return 1;
    config.name = "GraphView index lookup";
    config.operation = node_at_operation;
    config.verify = node_at_verify;
    if (!run_benchmark(&config)) return 1;
    config.name = "GraphView neighbor delegation";
    config.operation = neighbors_operation;
    config.verify = neighbors_verify;
    return run_benchmark(&config) ? 0 : 1;
}
