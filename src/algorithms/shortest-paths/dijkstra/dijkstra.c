// Declares the public Dijkstra API.
#include "dijkstra.h"
// Provides the priority frontier implementation.
#include "../../../data-structures/trees/heaps/binary-heap/binary_heap.h"
// Declares malloc and free.
#include <stdlib.h>

// Stores one pending shortest-path proposal in the min-heap frontier.
struct DijkstraNode {
    // Identifies the proposed graph Node.
    Node *node;
    // Stores the total source-to-Node path cost for this proposal.
    uint64_t total_distance;
};

// Shares the current settled Node state with GraphView neighbor relaxation.
struct DijkstraHeapContext {
    // Owns pending DijkstraNode proposals ordered by total distance.
    BinaryHeap *heap;
    // Records the current popped Node as a predecessor index.
    size_t parent_index;
    // Stores the current popped proposal's total distance.
    uint64_t current_distance;
    // Points to caller-owned best known distances.
    uint64_t *distances;
    // Points to caller-owned predecessor indexes.
    size_t *parents;
};

// Orders DijkstraNode proposals from smallest to largest total distance.
int compare(const void *left, const void *right) {
    const DijkstraNode *left_node = left;
    const DijkstraNode *right_node = right;
    if (left_node->total_distance > right_node->total_distance) {
        return 1;
    } else if (left_node->total_distance < right_node->total_distance) {
        return -1;
    } else {
        return 0;
    }
}

// Frees every remaining heap proposal before destroying the heap wrapper.
static void destroy_dijkstra_heap(BinaryHeap *heap) {
    void *entry = NULL;

    // Drains proposal allocations that BinaryHeap itself does not own.
    while (heap != NULL && !binary_heap_is_empty(heap)) {
        if (!binary_heap_pop(heap, &entry)) {
            break;
        }

        // Frees the Dijkstra proposal while leaving its graph-owned Node alone.
        free(entry);
    }

    // Releases binary-heap storage after all proposal entries are gone.
    binary_heap_destroy(heap);
}

// Relaxes one GraphView neighbor from the current settled Dijkstra proposal.
bool heap_push_neighbors(Node *node, uint64_t weight, void *context) {
    // Rejects an invalid graph neighbor.
    if (node == NULL) {
        return false;
    }

    // Restores the current relaxation state.
    DijkstraHeapContext *dijk_heap_context = context;

    // Rejects a path cost addition that would exceed uint64_t range.
    if (weight > DIJKSTRA_INFINITY - dijk_heap_context->current_distance) {
        return false;
    }

    // Calculates the candidate path cost through the current settled Node.
    uint64_t new_distance = dijk_heap_context->current_distance + weight;
    // Updates only when this route improves the neighbor's known best cost.
    if (new_distance < dijk_heap_context->distances[node->index]) {
        dijk_heap_context->parents[node->index] = dijk_heap_context->parent_index;
        dijk_heap_context->distances[node->index] = new_distance;

        // Allocates a persistent heap proposal for the improved route.
        DijkstraNode *dijk_node = malloc(sizeof(DijkstraNode));
        if (dijk_node == NULL) {
            return false;
        }

        // Stores the neighbor and its improved tentative total distance.
        dijk_node->node = node;
        dijk_node->total_distance = new_distance;
        // Transfers proposal ownership to the heap on successful push.
        if (!binary_heap_push(dijk_heap_context->heap, dijk_node)) {
            free(dijk_node);
            return false;
        }
    }

    // Allows GraphView to continue enumerating neighbors.
    return true;
}

// Computes shortest paths from source across all reachable nonnegative edges.
bool dijkstra(const GraphView *graph, Node *source, uint64_t *out_distances, size_t *out_parents) {
    // Rejects a missing or incomplete graph adapter.
    if (!graph_view_is_valid(graph)) {
        return false;
    }

    // Rejects a missing source Node.
    if (source == NULL) {
        return false;
    }

    // Reads dense Node count for output and settled arrays.
    size_t count = graph_view_vertex_count(graph);

    if (count == 0U) {
        return false;
    }

    // Rejects missing caller-owned distance output storage.
    if (out_distances == NULL) {
        return false;
    }

    // Rejects missing caller-owned predecessor output storage.
    if (out_parents == NULL) {
        return false;
    }

    // Initializes every Node as unreachable with no predecessor.
    size_t n = 0U;
    while(n < count) {
        out_distances[n] = DIJKSTRA_INFINITY;
        out_parents[n] = SIZE_MAX;
        n++;
    }

    // Validates source ownership through GraphView dense Node lookup.
    Node *out_node = NULL;

    if (!graph_view_node_at(graph, source->index, &out_node)) {
        return false;
    }

    if (out_node != source) {
        return false;
    }

    // Allocates final-distance tracking for stale heap-entry rejection.
    bool *settled = malloc(sizeof(bool) * count);
    if (settled == NULL) {
        return false;
    }

    // Marks every Node as not yet settled.
    n = 0U;
    while(n < count) {
        settled[n] = false;
        n++;
    }

    // Allocates the initial zero-distance source proposal.
    DijkstraNode *dijk_source = malloc(sizeof(DijkstraNode));
    if (dijk_source == NULL) {
        free(settled);
        return false;
    }

    dijk_source->node = source;
    dijk_source->total_distance = 0;

    // Creates the min-heap frontier of tentative shortest-path proposals.
    BinaryHeap *heap = binary_heap_create(compare);
    if (heap == NULL) {
        free(settled);
        free(dijk_source);
        return false;
    }

    // Pushes the source proposal into the frontier.
    if (!binary_heap_push(heap, dijk_source)) {
        free(settled);
        free(dijk_source);
        destroy_dijkstra_heap(heap);
        return false;
    }

    // Records source as the root of its own shortest-path tree.
    out_distances[source->index] = 0;
    out_parents[source->index] = source->index;

    // Receives generic heap items before treating them as Dijkstra proposals.
    void *node_void = NULL;
    DijkstraNode *popped = NULL;

    // Processes tentative proposals in nondecreasing total-distance order.
    while(!binary_heap_is_empty(heap)) {
        popped = NULL;
        node_void = NULL;

        // Pops the globally cheapest pending proposal.
        if (!binary_heap_pop(heap, &node_void)) {
            free(settled);
            destroy_dijkstra_heap(heap);
            return false;
        }

        // Restores the Dijkstra proposal from the generic heap item.
        popped = node_void;

        // Settles a Node only when this proposal is still its current best route.
        if (popped->total_distance == out_distances[popped->node->index] && !settled[popped->node->index]) {
            // Marks this shortest distance final before relaxing neighbors.
            settled[popped->node->index] = true;
            if (!graph_view_neighbors(graph, popped->node, heap_push_neighbors, &(DijkstraHeapContext){ .heap = heap, .parent_index = popped->node->index, .parents = out_parents, .current_distance = popped->total_distance, .distances = out_distances })) {
                free(settled);
                free(popped);
                destroy_dijkstra_heap(heap);
                return false;
            }
        }

        // Releases this consumed proposal without freeing its graph Node.
        free(popped);
    }

    // Releases traversal state after all reachable proposals are consumed.
    free(settled);
    destroy_dijkstra_heap(heap);

    // Reports successful all-reachable shortest-path computation.
    return true;
}
