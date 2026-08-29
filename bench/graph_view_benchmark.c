// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public GraphView API.
#include "../src/data-structures/graphs/graph-view/graph_view.h"

// Uses a substantial number of dense Nodes unless Make supplies a custom size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 10000U
#endif

// Defines the number of GraphView operations per benchmark sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };

// Stores state shared by GraphView wrapper benchmarks.
struct GraphViewContext {
    // Stores dense Nodes returned by the custom adapter.
    Node nodes[ITEM_COUNT];
    // Stores pointers to the dense Nodes for node_at lookups.
    Node *node_pointers[ITEM_COUNT];
    // Holds the GraphView used by every timed wrapper operation.
    GraphView view;
    // Tracks the next dense Node index used by timed operations.
    size_t next_index;
    // Receives a Node returned by the node_at wrapper.
    Node *out_node;
    // Counts visitor calls made by neighbor delegation.
    size_t visited_count;
    // Receives the count wrapper result.
    size_t vertex_count;
};

// Returns the custom adapter's fixed dense Node count.
static size_t vertex_count(const void *context) {
    (void)context;

    return ITEM_COUNT;
}

// Returns one custom adapter Node by its dense index.
static bool node_at(const void *context, size_t index, Node **out_node) {
    const struct GraphViewContext *view_context = context;

    if (index >= ITEM_COUNT || out_node == NULL) {
        return false;
    }

    *out_node = view_context->node_pointers[index];
    return true;
}

// Visits one synthetic successor neighbor for each dense Node.
static bool neighbors(const void *context, const Node *node,
                      GraphViewVisitFn visit, void *visit_context) {
    const struct GraphViewContext *view_context = context;
    size_t next_index = node->index + 1U;

    if (next_index == ITEM_COUNT) {
        next_index = 0U;
    }

    return visit(view_context->node_pointers[next_index], 1U, visit_context);
}

// Records one neighbor visit delegated through GraphView.
static bool record_neighbor(Node *neighbor, uint64_t weight, void *context) {
    struct GraphViewContext *view_context = context;

    if (neighbor == NULL || weight != 1U) {
        return false;
    }

    view_context->visited_count++;
    return true;
}

// Resets per-sample operation state outside the timed region.
static bool setup(void *context) {
    struct GraphViewContext *view_context = context;

    view_context->next_index = 0U;
    view_context->out_node = NULL;
    view_context->visited_count = 0U;
    view_context->vertex_count = 0U;
    return true;
}

// Reads the GraphView node count once.
static bool vertex_count_operation(void *context) {
    struct GraphViewContext *view_context = context;

    view_context->vertex_count = graph_view_vertex_count(&view_context->view);
    return view_context->vertex_count == ITEM_COUNT;
}

// Looks up one dense Node through GraphView.
static bool node_at_operation(void *context) {
    struct GraphViewContext *view_context = context;

    if (!graph_view_node_at(
            &view_context->view,
            view_context->next_index,
            &view_context->out_node
        )) {
        return false;
    }

    view_context->next_index++;
    return true;
}

// Visits one synthetic neighbor through GraphView.
static bool neighbors_operation(void *context) {
    struct GraphViewContext *view_context = context;

    if (!graph_view_neighbors(
            &view_context->view,
            view_context->node_pointers[view_context->next_index],
            record_neighbor,
            view_context
        )) {
        return false;
    }

    view_context->next_index++;
    return true;
}

// Confirms repeated count reads returned the expected result.
static bool vertex_count_verify(void *context) {
    struct GraphViewContext *view_context = context;

    return view_context->vertex_count == ITEM_COUNT;
}

// Confirms every dense Node lookup completed.
static bool node_at_verify(void *context) {
    struct GraphViewContext *view_context = context;

    return view_context->next_index == ITEM_COUNT && view_context->out_node != NULL;
}

// Confirms every neighbor operation invoked the visitor once.
static bool neighbors_verify(void *context) {
    struct GraphViewContext *view_context = context;

    return view_context->next_index == ITEM_COUNT &&
        view_context->visited_count == ITEM_COUNT;
}

// Runs and prints one configured GraphView benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    static struct GraphViewContext view_context;
    BenchmarkConfig config = {
        .name = "GraphView vertex count",
        .warmup_iterations = ITEM_COUNT,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = setup,
        .operation = vertex_count_operation,
        .verify = vertex_count_verify,
        .teardown = NULL,
        .context = &view_context
    };

    // Initializes stable dense indexes and the adapter's Node pointer table.
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        view_context.nodes[index].index = index;
        view_context.node_pointers[index] = &view_context.nodes[index];
    }
    view_context.view.context = &view_context;
    view_context.view.vertex_count = vertex_count;
    view_context.view.node_at = node_at;
    view_context.view.neighbors = neighbors;

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "GraphView node at";
    config.operation = node_at_operation;
    config.verify = node_at_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "GraphView neighbors";
    config.operation = neighbors_operation;
    config.verify = neighbors_verify;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
