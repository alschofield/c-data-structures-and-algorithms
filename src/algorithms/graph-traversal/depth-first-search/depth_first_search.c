// Declares the public DFS API.
#include "depth_first_search.h"
// Declares malloc and free.
#include <stdlib.h>
#include "../../../data-structures/linear/stacks/stack/stack.h"

// Stores DFS frontier and discovered-state shared by neighbor enumeration.
struct DepthFirstSearchStackContext {
    // Owns the FILO frontier of discovered Nodes awaiting traversal.
    Stack *stack;
    // Marks dense Node indexes already stacked into the frontier.
    bool *stacked;
};

// Marks and pushes one unvisited neighbor discovered through GraphView.
static bool stack_push_neighbor(Node *node, uint64_t weight, void *context) {
    // Rejects an invalid neighbor before reading its dense index.
    if (node == NULL) {
        return false;
    }

    // DFS ignores edge weights while preserving GraphView callback compatibility.
    (void)(weight);

    // Restores the stack and discovered-state context for this neighbor visit.
    DepthFirstSearchStackContext *dfs_stack_context = context;

    // Pushes every reachable Node at most once, including on cyclic graphs.
    if (!dfs_stack_context->stacked[node->index]) {
        // Marks discovery before push so alternate edges cannot duplicate it.
        dfs_stack_context->stacked[node->index] = true;
        // Pushes the discovered Node onto the LIFO DFS frontier.
        if (!stack_push(dfs_stack_context->stack, node)) {
            return false;
        }
    }

    // Allows GraphView to continue enumerating remaining neighbors.
    return true;
}

// Traverses Nodes reachable from source in LIFO depth-first order.
bool depth_first_search(const GraphView *graph, Node *source, DepthFirstSearchVisitFn visit, void *context) {
    // Rejects a missing or incomplete graph adapter.
    if (!graph_view_is_valid(graph)) {
        return false;
    }

    // Reads the dense Node count used to size discovered-state tracking.
    size_t count = graph_view_vertex_count(graph);

    // Treats an empty graph as having no valid traversal source.
    if (count == 0U) {
        return false;
    }

    // Rejects a missing traversal source.
    if (source == NULL) {
        return false;
    }

    // Rejects a missing caller-provided visit action.
    if (visit == NULL) {
        return false;
    }

    // Verifies that source is the graph-owned Node at its claimed dense index.
    Node *out_node = NULL;

    if(!graph_view_node_at(graph, source->index, &out_node)) {
        return false;
    }

    // Rejects a foreign Node that happens to have an in-range index.
    if (out_node != source) {
        return false;
    }

    // Allocates dense discovered-state tracking for cycle-safe traversal.
    bool *stacked = malloc(sizeof(bool) * count);
    if (stacked == NULL) {
        return false;
    }

    // Initializes every dense Node as not yet pushed.
    size_t n = 0U;
    while (n < count) {
        stacked[n] = false;
        n++;
    }

    // Creates the LIFO frontier used to preserve depth-first order.
    Stack *stack = stack_create();
    if (stack == NULL) {
        free(stacked);
        return false;
    }

    // Seeds DFS with the source Node.
    if (!stack_push(stack, source)) {
        free(stacked);
        stack_destroy(stack);
        return false;
    }

    // Marks source immediately so cyclic edges cannot re-push it.
    stacked[source->index] = true;

    // Receives each generic stack item before treating it as a Node pointer.
    void *node_void = NULL;

    // Processes the frontier until every reachable Node is visited or stopped.
    while(!stack_is_empty(stack)) {
        // Pops the most recently discovered Node from the LIFO frontier.
        if (!stack_pop(stack, &node_void)) {
            free(stacked);
            stack_destroy(stack);
            return false;
        }

        // Restores the graph-specific Node pointer from the generic stack item.
        Node *node = node_void;

        // Lets caller state record this Node or request early traversal stop.
        if (!visit(node, context)) {
            free(stacked);
            stack_destroy(stack);
            return false;
        }

        // Discovers and pushes every unvisited outgoing neighbor of this Node.
        if (!graph_view_neighbors(graph, node, stack_push_neighbor, &(DepthFirstSearchStackContext){ .stack = stack, .stacked = stacked })) {
            free(stacked);
            stack_destroy(stack);
            return false;
        }
    }

    // Releases traversal-only state after the reachable frontier is exhausted.
    free(stacked);
    stack_destroy(stack);

    // Reports successful full traversal.
    return true;
}
