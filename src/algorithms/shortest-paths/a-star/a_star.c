// Declares the public A-star API.
#include "a_star.h"
// Provides the min-priority frontier implementation.
#include "../../../data-structures/trees/heaps/binary-heap/binary_heap.h"
// Declares malloc and free.
#include <stdlib.h>

// Stores one pending A-star route proposal in the min-heap frontier.
struct AStarNode {
    // Identifies the proposed graph Node.
    Node *node;
    // Stores the known source-to-Node route cost.
    uint64_t g_score;
    // Stores the priority: known route cost plus heuristic estimate.
    uint64_t f_score;
};

// Shares the current expansion state with GraphView neighbor relaxation.
struct AStarHeapContext {
    // Owns pending A-star proposals ordered by f-score.
    BinaryHeap *heap;
    // Records the current expanded Node as a predecessor index.
    size_t parent_index;
    // Stores the current expanded proposal's known route cost.
    uint64_t current_g_score;
    // Points to the best known route cost for each graph Node.
    uint64_t *g_scores;
    // Points to predecessor indexes used to reconstruct the final path.
    size_t *parents;
    // Estimates the remaining cost from a Node to the goal.
    AStarHeuristicFn heuristic;
    // Carries caller-owned heuristic state without taking ownership.
    void *heuristic_context;
};

// Orders A-star proposals by f-score, then g-score, then Node index.
static int compare(const void *left, const void *right) {
    // Restores typed proposals from BinaryHeap's generic item pointers.
    const AStarNode *left_node = left;
    const AStarNode *right_node = right;

    // Prefers the route with the smallest estimated total cost.
    if (left_node->f_score < right_node->f_score) {
        return -1;
    }
    if (left_node->f_score > right_node->f_score) {
        return 1;
    }

    // Breaks equal estimates by preferring the cheaper known route.
    if (left_node->g_score < right_node->g_score) {
        return -1;
    }
    if (left_node->g_score > right_node->g_score) {
        return 1;
    }

    // Makes otherwise identical proposals deterministic.
    if (left_node->node->index < right_node->node->index) {
        return -1;
    }
    if (left_node->node->index > right_node->node->index) {
        return 1;
    }

    return 0;
}

// Relaxes one neighbor from the current A-star expansion.
static bool a_star_push_neighbors(Node *node, uint64_t weight, void *context) {
    // Rejects an invalid graph neighbor.
    if (node == NULL) {
        return false;
    }

    // Restores the current expansion state.
    AStarHeapContext *a_star_heap_context = context;

    // Rejects a route-cost addition that exceeds uint64_t range.
    if (weight > UINT64_MAX - a_star_heap_context->current_g_score) {
        return false;
    }

    // Calculates the route cost of reaching this neighbor through the current Node.
    uint64_t candidate_g = a_star_heap_context->current_g_score + weight;

    // Ignores routes that do not improve the neighbor's known best route cost.
    if (candidate_g >= a_star_heap_context->g_scores[node->index]) {
        return true;
    }

    // Gets the caller-supplied estimate from this neighbor to the goal.
    uint64_t candidate_h = a_star_heap_context->heuristic(
        node->index,
        a_star_heap_context->heuristic_context
    );

    // Rejects a priority calculation that exceeds uint64_t range.
    if (candidate_g > UINT64_MAX - candidate_h) {
        return false;
    }

    // Combines known route cost and remaining-cost estimate for heap priority.
    uint64_t candidate_f = candidate_g + candidate_h;

    // Allocates a persistent heap proposal for the improved route.
    AStarNode *a_star_node = malloc(sizeof(AStarNode));
    if (a_star_node == NULL) {
        return false;
    }

    // Stores the proposed graph Node and its two route scores.
    a_star_node->node = node;
    a_star_node->g_score = candidate_g;
    a_star_node->f_score = candidate_f;

    // Transfers proposal ownership to the heap on a successful push.
    if (!binary_heap_push(a_star_heap_context->heap, a_star_node)) {
        free(a_star_node);
        return false;
    }

    // Commits the improved route only after its proposal is queued successfully.
    a_star_heap_context->g_scores[node->index] = candidate_g;
    a_star_heap_context->parents[node->index] = a_star_heap_context->parent_index;
    return true;
}

// Frees every remaining heap proposal before destroying the heap wrapper.
static void destroy_a_star_heap(BinaryHeap *heap) {
    // Receives each caller-owned heap proposal before releasing it.
    void *entry = NULL;

    // Drains proposal allocations that BinaryHeap itself does not own.
    while (heap != NULL && !binary_heap_is_empty(heap)) {
        if (!binary_heap_pop(heap, &entry)) {
            break;
        }

        // Frees the A-star proposal while leaving its graph-owned Node alone.
        free(entry);
    }

    // Releases binary-heap storage after all proposal entries are gone.
    binary_heap_destroy(heap);
}

// Finds an optimal source-to-goal path with an admissible heuristic.
bool a_star(
    const GraphView *graph,
    Node *source,
    Node *goal,
    AStarHeuristicFn heuristic,
    void *context,
    size_t *out_path,
    size_t out_path_capacity,
    size_t *out_path_length
) {
    // Rejects a missing or incomplete graph adapter.
    if (!graph_view_is_valid(graph)) {
        return false;
    }

    // Rejects missing endpoint Nodes.
    if (source == NULL || goal == NULL) {
        return false;
    }

    // Reads the dense graph Node count used by internal arrays.
    size_t count = graph_view_vertex_count(graph);
    if (count == 0U) {
        return false;
    }

    // Rejects missing caller-supplied path and heuristic storage.
    if (heuristic == NULL || out_path == NULL || out_path_length == NULL) {
        return false;
    }

    // Every successful path has at least its source Node.
    if (out_path_capacity == 0U) {
        return false;
    }

    // Validates that source is owned by this GraphView.
    Node *out_node = NULL;
    if (!graph_view_node_at(graph, source->index, &out_node) || out_node != source) {
        return false;
    }

    // Validates that goal is owned by this GraphView.
    out_node = NULL;
    if (!graph_view_node_at(graph, goal->index, &out_node) || out_node != goal) {
        return false;
    }

    // Returns the one-Node zero-cost path without allocating search state.
    if (source == goal) {
        out_path[0] = source->index;
        *out_path_length = 1U;
        return true;
    }

    // Allocates predecessor indexes for later source-to-goal reconstruction.
    size_t *parents = malloc(sizeof(size_t) * count);
    if (parents == NULL) {
        return false;
    }

    // Allocates best known source-to-Node costs for stale-entry rejection.
    uint64_t *g_scores = malloc(sizeof(uint64_t) * count);
    if (g_scores == NULL) {
        free(parents);
        return false;
    }

    // Marks every Node unreachable and parentless before the search begins.
    for (size_t index = 0U; index < count; index++) {
        g_scores[index] = UINT64_MAX;
        parents[index] = SIZE_MAX;
    }

    // Allocates the initial zero-cost source proposal.
    AStarNode *a_star_source = malloc(sizeof(AStarNode));
    if (a_star_source == NULL) {
        free(g_scores);
        free(parents);
        return false;
    }

    // Stores the source and its heuristic-only frontier priority.
    a_star_source->node = source;
    a_star_source->g_score = 0U;
    a_star_source->f_score = heuristic(source->index, context);

    // Creates the min-heap frontier of tentative path proposals.
    BinaryHeap *heap = binary_heap_create(compare);
    if (heap == NULL) {
        free(a_star_source);
        free(g_scores);
        free(parents);
        return false;
    }

    // Pushes the source proposal before exposing its score as current.
    if (!binary_heap_push(heap, a_star_source)) {
        free(a_star_source);
        destroy_a_star_heap(heap);
        free(g_scores);
        free(parents);
        return false;
    }

    // Records the source as the only initially reachable Node.
    g_scores[source->index] = 0U;

    // Records whether the goal was extracted as a current best proposal.
    bool found_goal = false;

    // Processes the frontier from lowest estimated total cost upward.
    while (!binary_heap_is_empty(heap)) {
        // Receives the generic heap item before treating it as an A-star proposal.
        void *entry = NULL;
        if (!binary_heap_pop(heap, &entry)) {
            destroy_a_star_heap(heap);
            free(g_scores);
            free(parents);
            return false;
        }

        // Restores the typed proposal from the generic heap item.
        AStarNode *popped = entry;

        // Discards a proposal superseded by a later cheaper route.
        if (popped->g_score != g_scores[popped->node->index]) {
            free(popped);
            continue;
        }

        // Stops only after the goal becomes the best current frontier proposal.
        if (popped->node == goal) {
            found_goal = true;
            free(popped);
            break;
        }

        // Relaxes outgoing edges without permanently settling Nodes.
        if (!graph_view_neighbors(
                graph,
                popped->node,
                a_star_push_neighbors,
                &(AStarHeapContext) {
                    .heap = heap,
                    .parent_index = popped->node->index,
                    .current_g_score = popped->g_score,
                    .g_scores = g_scores,
                    .parents = parents,
                    .heuristic = heuristic,
                    .heuristic_context = context
                }
            )) {
            free(popped);
            destroy_a_star_heap(heap);
            free(g_scores);
            free(parents);
            return false;
        }

        // Releases the consumed proposal without freeing its graph Node.
        free(popped);
    }

    // Reports a missing route after every reachable proposal has been considered.
    if (!found_goal) {
        destroy_a_star_heap(heap);
        free(g_scores);
        free(parents);
        return false;
    }

    // Counts the goal-to-source parent chain before writing caller output.
    size_t path_length = 1U;
    size_t current = goal->index;
    while (current != source->index) {
        // Rejects a malformed parent chain instead of reading outside parents.
        if (parents[current] == SIZE_MAX || path_length == count) {
            destroy_a_star_heap(heap);
            free(g_scores);
            free(parents);
            return false;
        }

        // Follows one predecessor toward source and accounts for it.
        current = parents[current];
        path_length++;
    }

    // Rejects a caller buffer too small for the complete returned route.
    if (path_length > out_path_capacity) {
        destroy_a_star_heap(heap);
        free(g_scores);
        free(parents);
        return false;
    }

    // Writes the reverse parent chain from the end to return source-to-goal order.
    current = goal->index;
    for (size_t write_index = path_length; write_index > 0U; write_index--) {
        out_path[write_index - 1U] = current;
        if (current != source->index) {
            current = parents[current];
        }
    }

    // Publishes the exact number of path indexes written to caller storage.
    *out_path_length = path_length;

    // Releases search-only storage after producing the caller-owned path.
    destroy_a_star_heap(heap);
    free(g_scores);
    free(parents);
    return true;
}
