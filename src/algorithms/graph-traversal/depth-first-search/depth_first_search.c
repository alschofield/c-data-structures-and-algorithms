// Declares the public DFS API.
#include "depth_first_search.h"
// Provides the LIFO frontier implementation.
#include "../../../data-structures/linear/stacks/stack/stack.h"
// Declares malloc and free.
#include <stdlib.h>

// Stores DFS frontier and discovered-state shared by neighbor enumeration.
struct DepthFirstSearchStackContext {
    // Owns the LIFO frontier of discovered indexes awaiting traversal.
    Stack *stack;
    // Marks dense indexes already pushed into the frontier.
    bool *stacked;
    // Stores stable index values whose addresses can be held by Stack.
    size_t *indexes;
};

// Marks and pushes one unvisited neighbor discovered through GraphView.
static bool stack_push_neighbor(size_t neighbor_index, uint64_t weight,
                                void *context) {
    (void)weight;

    DepthFirstSearchStackContext *dfs_context = context;
    if (!dfs_context->stacked[neighbor_index]) {
        dfs_context->stacked[neighbor_index] = true;
        if (!stack_push(dfs_context->stack, &dfs_context->indexes[neighbor_index])) {
            return false;
        }
    }

    return true;
}

// Traverses indexes reachable from source in LIFO depth-first order.
bool depth_first_search(const GraphView *graph, size_t source,
                        DepthFirstSearchVisitFn visit, void *context) {
    if (!graph_view_is_valid(graph) || visit == NULL) {
        return false;
    }

    size_t count = graph_view_vertex_count(graph);
    if (count == 0U || source >= count || !graph_view_node_at(graph, source)) {
        return false;
    }

    bool *stacked = malloc(sizeof(*stacked) * count);
    size_t *indexes = malloc(sizeof(*indexes) * count);
    if (stacked == NULL || indexes == NULL) {
        free(stacked);
        free(indexes);
        return false;
    }

    for (size_t index = 0U; index < count; index++) {
        stacked[index] = false;
        indexes[index] = index;
    }

    Stack *stack = stack_create();
    if (stack == NULL) {
        free(stacked);
        free(indexes);
        return false;
    }

    if (!stack_push(stack, &indexes[source])) {
        stack_destroy(stack);
        free(stacked);
        free(indexes);
        return false;
    }
    stacked[source] = true;

    while (!stack_is_empty(stack)) {
        void *entry = NULL;
        if (!stack_pop(stack, &entry)) {
            stack_destroy(stack);
            free(stacked);
            free(indexes);
            return false;
        }

        size_t node_index = *(size_t *)entry;
        if (!visit(node_index, context) || !graph_view_neighbors(
                graph,
                node_index,
                stack_push_neighbor,
                &(DepthFirstSearchStackContext) {
                    .stack = stack,
                    .stacked = stacked,
                    .indexes = indexes
                }
            )) {
            stack_destroy(stack);
            free(stacked);
            free(indexes);
            return false;
        }
    }

    stack_destroy(stack);
    free(stacked);
    free(indexes);
    return true;
}
