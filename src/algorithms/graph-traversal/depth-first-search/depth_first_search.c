#include "depth_first_search.h"
#include "../../../data-structures/linear/stacks/stack/stack.h"

struct DepthFirstSearchStackContext {
    // Owns the FILO frontier of discovered Nodes awaiting traversal.
    Stack *stack;
    // Marks dense Node indexes already stacked into the frontier.
    bool *stacked;
};

static bool stack_push_neighbor(Node *node, uint32_t weight, void *context) {
    if (node == NULL) {
        return false;
    }

    DepthFirstSearchStackContext *dfs_stack_context = context;

    if (!dfs_stack_context->stacked[node->index]) {
        dfs_stack_context->stacked[node->index] = true;
        if (!stack_push(dfs_stack_context->stack, node)) {
            return false;
        }
    }

    return true;
}

bool depth_first_search(const GraphView *graph, Node *source, DepthFirstSearchVisitFn visit, void *context) {
    if (!graph_view_is_valid(graph)) {
        return false;
    }

    size_t count = graph_view_vertex_count(graph);

    if (count == 0U) {
        return false;
    }

    if (source == NULL) {
        return false;
    }

    if (visit == NULL) {
        return false;
    }

    Node *out_node = NULL;

    if(!graph_view_node_at(graph, source->index, &out_node)) {
        return false;
    }

    if (out_node != source) {
        return false;
    }

    bool *stacked = malloc(sizeof(bool) * count);
    if (stacked == NULL) {
        return false;
    }

    size_t n = 0U;
    while (n < count) {
        stacked[n] = false;
        n++;
    }

    Stack *stack = create_stack();
    if (stack == NULL) {
        free(stacked);
        return false;
    }

    if (!stack_push(stack, source)) {
        free(stacked);
        stack_destroy(stack);
        return false;
    }

    stacked[source->index] = true;

    void *node_void = NULL;

    while(!stack_is_empty(stack)) {
        if (!stack_pop(stack, &node_void)) {
            free(stacked);
            stack_destroy(stack);
        }

        Node *node = node_void;

        if (!visit(node, context)) {
            free(stacked);
            stack_destroy(stack);
            return true;
        }

        if (!graph_view_neighbors(graph, node, stack_push_neighbor, &(DepthFirstSearchStackContext){ .stack = stack, .stacked = stacked })) {
            free(stacked);
            stack_destroy(stack);
            return false;
        }
    }

    free(stacked);
    stack_destroy(stack);

    return true;
}
