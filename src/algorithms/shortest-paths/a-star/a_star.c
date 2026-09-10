// Declares the public A-star API.
#include "a_star.h"
// Provides the min-priority frontier implementation.
#include "../../../data-structures/trees/heaps/binary-heap/binary_heap.h"
// Declares malloc and free.
#include <stdlib.h>

// Stores one pending A-star route proposal in the min-heap frontier.
struct AStarNode {
    size_t index;
    uint64_t g_score;
    uint64_t f_score;
};

// Shares current expansion state with GraphView neighbor relaxation.
struct AStarHeapContext {
    BinaryHeap *heap;
    size_t parent_index;
    uint64_t current_g_score;
    uint64_t *g_scores;
    size_t *parents;
    AStarHeuristicFn heuristic;
    void *heuristic_context;
};

// Orders A-star proposals by f-score, then g-score, then index.
static int compare(const void *left, const void *right) {
    const AStarNode *left_node = left;
    const AStarNode *right_node = right;

    if (left_node->f_score < right_node->f_score) return -1;
    if (left_node->f_score > right_node->f_score) return 1;
    if (left_node->g_score < right_node->g_score) return -1;
    if (left_node->g_score > right_node->g_score) return 1;
    if (left_node->index < right_node->index) return -1;
    if (left_node->index > right_node->index) return 1;
    return 0;
}

// Frees every remaining heap proposal before destroying the heap wrapper.
static void destroy_a_star_heap(BinaryHeap *heap) {
    void *entry = NULL;

    while (heap != NULL && !binary_heap_is_empty(heap)) {
        if (!binary_heap_pop(heap, &entry)) break;
        free(entry);
    }
    binary_heap_destroy(heap);
}

// Relaxes one index neighbor from the current A-star expansion.
static bool a_star_push_neighbors(size_t neighbor_index, uint64_t weight,
                                  void *context) {
    AStarHeapContext *heap_context = context;

    if (weight > UINT64_MAX - heap_context->current_g_score) return false;
    uint64_t candidate_g = heap_context->current_g_score + weight;
    if (candidate_g >= heap_context->g_scores[neighbor_index]) return true;

    uint64_t heuristic = heap_context->heuristic(
        neighbor_index,
        heap_context->heuristic_context
    );
    if (candidate_g > UINT64_MAX - heuristic) return false;

    AStarNode *proposal = malloc(sizeof(*proposal));
    if (proposal == NULL) return false;
    proposal->index = neighbor_index;
    proposal->g_score = candidate_g;
    proposal->f_score = candidate_g + heuristic;
    if (!binary_heap_push(heap_context->heap, proposal)) {
        free(proposal);
        return false;
    }

    heap_context->g_scores[neighbor_index] = candidate_g;
    heap_context->parents[neighbor_index] = heap_context->parent_index;
    return true;
}

// Finds an optimal source-to-goal path with an admissible heuristic.
bool a_star(const GraphView *graph, size_t source, size_t goal,
            AStarHeuristicFn heuristic, void *context,
            size_t *out_path, size_t out_path_capacity,
            size_t *out_path_length) {
    if (!graph_view_is_valid(graph) || heuristic == NULL || out_path == NULL ||
        out_path_length == NULL || out_path_capacity == 0U) return false;

    size_t count = graph_view_vertex_count(graph);
    if (count == 0U || source >= count || goal >= count ||
        !graph_view_node_at(graph, source) || !graph_view_node_at(graph, goal)) return false;

    if (source == goal) {
        out_path[0] = source;
        *out_path_length = 1U;
        return true;
    }

    size_t *parents = malloc(sizeof(*parents) * count);
    uint64_t *g_scores = malloc(sizeof(*g_scores) * count);
    if (parents == NULL || g_scores == NULL) {
        free(parents);
        free(g_scores);
        return false;
    }
    for (size_t index = 0U; index < count; index++) {
        parents[index] = SIZE_MAX;
        g_scores[index] = UINT64_MAX;
    }

    BinaryHeap *heap = binary_heap_create(compare);
    AStarNode *initial = malloc(sizeof(*initial));
    if (heap == NULL || initial == NULL) {
        binary_heap_destroy(heap);
        free(initial);
        free(parents);
        free(g_scores);
        return false;
    }
    initial->index = source;
    initial->g_score = 0U;
    initial->f_score = heuristic(source, context);
    if (!binary_heap_push(heap, initial)) {
        free(initial);
        destroy_a_star_heap(heap);
        free(parents);
        free(g_scores);
        return false;
    }
    g_scores[source] = 0U;

    bool found_goal = false;
    while (!binary_heap_is_empty(heap)) {
        void *entry = NULL;
        if (!binary_heap_pop(heap, &entry)) {
            destroy_a_star_heap(heap);
            free(parents);
            free(g_scores);
            return false;
        }

        AStarNode *proposal = entry;
        if (proposal->g_score == g_scores[proposal->index]) {
            if (proposal->index == goal) {
                found_goal = true;
                free(proposal);
                break;
            }
            if (!graph_view_neighbors(
                    graph,
                    proposal->index,
                    a_star_push_neighbors,
                    &(AStarHeapContext) {
                        .heap = heap,
                        .parent_index = proposal->index,
                        .current_g_score = proposal->g_score,
                        .g_scores = g_scores,
                        .parents = parents,
                        .heuristic = heuristic,
                        .heuristic_context = context
                    }
                )) {
                free(proposal);
                destroy_a_star_heap(heap);
                free(parents);
                free(g_scores);
                return false;
            }
        }
        free(proposal);
    }

    if (!found_goal) {
        destroy_a_star_heap(heap);
        free(parents);
        free(g_scores);
        return false;
    }

    size_t path_length = 1U;
    size_t current = goal;
    while (current != source) {
        if (parents[current] == SIZE_MAX || path_length == count) {
            destroy_a_star_heap(heap);
            free(parents);
            free(g_scores);
            return false;
        }
        current = parents[current];
        path_length++;
    }
    if (path_length > out_path_capacity) {
        destroy_a_star_heap(heap);
        free(parents);
        free(g_scores);
        return false;
    }

    current = goal;
    for (size_t write_index = path_length; write_index > 0U; write_index--) {
        out_path[write_index - 1U] = current;
        if (current != source) current = parents[current];
    }
    *out_path_length = path_length;

    destroy_a_star_heap(heap);
    free(parents);
    free(g_scores);
    return true;
}
