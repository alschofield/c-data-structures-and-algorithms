// Declares the public Dijkstra API.
#include "dijkstra.h"
// Provides the priority frontier implementation.
#include "../../../data-structures/trees/heaps/binary-heap/binary_heap.h"
// Declares malloc and free.
#include <stdlib.h>

// Stores one pending shortest-path proposal in the min-heap frontier.
struct DijkstraNode {
    // Identifies the proposed dense graph index.
    size_t index;
    // Stores the total source-to-index path cost for this proposal.
    uint64_t total_distance;
};

// Shares the current settled index state with GraphView neighbor relaxation.
struct DijkstraHeapContext {
    BinaryHeap *heap;
    size_t parent_index;
    uint64_t current_distance;
    uint64_t *distances;
    size_t *parents;
};

// Orders Dijkstra proposals from smallest to largest total distance.
static int compare(const void *left, const void *right) {
    const DijkstraNode *left_node = left;
    const DijkstraNode *right_node = right;

    if (left_node->total_distance < right_node->total_distance) {
        return -1;
    }
    if (left_node->total_distance > right_node->total_distance) {
        return 1;
    }
    if (left_node->index < right_node->index) {
        return -1;
    }
    if (left_node->index > right_node->index) {
        return 1;
    }
    return 0;
}

// Frees every remaining heap proposal before destroying the heap wrapper.
static void destroy_dijkstra_heap(BinaryHeap *heap) {
    void *entry = NULL;

    while (heap != NULL && !binary_heap_is_empty(heap)) {
        if (!binary_heap_pop(heap, &entry)) {
            break;
        }
        free(entry);
    }
    binary_heap_destroy(heap);
}

// Relaxes one GraphView neighbor from the current settled proposal.
static bool heap_push_neighbors(size_t neighbor_index, uint64_t weight,
                                void *context) {
    DijkstraHeapContext *heap_context = context;

    if (weight > DIJKSTRA_INFINITY - heap_context->current_distance) {
        return false;
    }

    uint64_t distance = heap_context->current_distance + weight;
    if (distance >= heap_context->distances[neighbor_index]) {
        return true;
    }

    DijkstraNode *proposal = malloc(sizeof(*proposal));
    if (proposal == NULL) {
        return false;
    }
    proposal->index = neighbor_index;
    proposal->total_distance = distance;

    if (!binary_heap_push(heap_context->heap, proposal)) {
        free(proposal);
        return false;
    }

    heap_context->distances[neighbor_index] = distance;
    heap_context->parents[neighbor_index] = heap_context->parent_index;
    return true;
}

// Computes shortest paths from source across all reachable nonnegative edges.
bool dijkstra(const GraphView *graph, size_t source,
              uint64_t *out_distances, size_t *out_parents) {
    if (!graph_view_is_valid(graph) || out_distances == NULL || out_parents == NULL) {
        return false;
    }

    size_t count = graph_view_vertex_count(graph);
    if (count == 0U || source >= count || !graph_view_node_at(graph, source)) {
        return false;
    }

    for (size_t index = 0U; index < count; index++) {
        out_distances[index] = DIJKSTRA_INFINITY;
        out_parents[index] = SIZE_MAX;
    }

    bool *settled = malloc(sizeof(*settled) * count);
    if (settled == NULL) {
        return false;
    }
    for (size_t index = 0U; index < count; index++) {
        settled[index] = false;
    }

    BinaryHeap *heap = binary_heap_create(compare);
    DijkstraNode *initial = malloc(sizeof(*initial));
    if (heap == NULL || initial == NULL) {
        binary_heap_destroy(heap);
        free(initial);
        free(settled);
        return false;
    }
    initial->index = source;
    initial->total_distance = 0U;
    if (!binary_heap_push(heap, initial)) {
        free(initial);
        destroy_dijkstra_heap(heap);
        free(settled);
        return false;
    }

    out_distances[source] = 0U;
    out_parents[source] = source;

    while (!binary_heap_is_empty(heap)) {
        void *entry = NULL;
        if (!binary_heap_pop(heap, &entry)) {
            destroy_dijkstra_heap(heap);
            free(settled);
            return false;
        }

        DijkstraNode *proposal = entry;
        if (proposal->total_distance == out_distances[proposal->index] &&
            !settled[proposal->index]) {
            settled[proposal->index] = true;
            if (!graph_view_neighbors(
                    graph,
                    proposal->index,
                    heap_push_neighbors,
                    &(DijkstraHeapContext) {
                        .heap = heap,
                        .parent_index = proposal->index,
                        .current_distance = proposal->total_distance,
                        .distances = out_distances,
                        .parents = out_parents
                    }
                )) {
                free(proposal);
                destroy_dijkstra_heap(heap);
                free(settled);
                return false;
            }
        }
        free(proposal);
    }

    destroy_dijkstra_heap(heap);
    free(settled);
    return true;
}
