#include "dijkstra.h"
#include "../../../data-structures/trees/heaps/binary-heap/binary_heap.h"

struct DijkstraNode {
    Node *node;
    uint64_t total_distance;
};

struct DijkstraHeapContext {
    BinaryHeap *heap;
    uint64_t *distances;
    Node **parents;
};

int compare(const void *left, const void *right) {
    DijkstraNode *left_node = left;
    DijkstraNode *right_node = right;
    return right_node->total_distance - left_node->total_distance;
}

bool heap_push_neighbors(Node *node, uint64_t weight, void *context) {
    if (node == NULL) {
        return false;
    }

    DijkstraHeapContext *djik_heap_context = context;

    // idk how correct this is but it should be something like this
    uint64_t new_distance = djik_heap_context->distances[node->index] + weight;
    if (new_distance > djik_heap_context->distances[node->index]) {
        djik_heap_context->parents[node->index] = node;
        djik_heap_context->distances[node->index] = new_distance;

        if (!binary_heap_push(djik_heap_context->heap, &(DijkstraNode){ .node = node, .total_distance = new_distance })) {
            return false;
        }
    }

    return true;
}

bool dijkstra(const GraphView *graph, Node *source, uint64_t *out_distances, size_t *out_parents) {
    if (!graph_view_is_valid(graph)) {
        return false;
    }

    size_t count = graph_view_vertex_count(graph);

    if (count == 0U) {
        return false;
    }

    if (out_distances == NULL) {
        return false;
    }

    if (out_parents == NULL) {
        return false;
    }

    Node *out_node = NULL;

    if (!graph_view_node_at(graph, source->index, &out_node)) {
        return false;
    }

    if (out_node != source) {
        return false;
    }

    bool *settled = malloc(sizeof(bool) * count);
    if (settled == NULL) {
        return false;
    }

    size_t n = 0U;
    while(n < count) {
        settled[n] = false;
        n++;
    }

    uint64_t *distances = malloc(sizeof(int) * count);
    if (distances == NULL) {
        free(settled);
        return false;
    }

    n = 0U;
    while (n < count) {
        distances[n] = DIJKSTRA_INFINITY;
        n++;
    }

    Node **parents = malloc(sizeof(Node) * count);
    if (parents == NULL) {
        free(settled);
        free(distances);
        return false;
    }

    n = 0U;
    while(n < count) {
        parents[n] = NULL;
        n++;
    }

    BinaryHeap *heap = binary_heap_create(compare);
    if (heap == NULL) {
        free(settled);
        free(distances);
        free(parents);
        return false;
    }

    if (!binary_heap_push(heap, &(DijkstraNode){ .node = source, .total_distance = 0 })) {
        free(settled);
        free(distances);
        free(parents);
        binary_heap_destroy(heap);
        return false;
    }

    settled[source->index] = true;
    distances[source->index] = 0;
    parents[source->index] = source;

    void *node_void = NULL;
    out_node = NULL;

    while(!binary_heap_is_empty(heap)) {
        out_node = NULL;
        node_void = NULL;

        if (!binary_heap_pop(heap, node_void)) {
            free(settled);
            free(distances);
            free(parents);
            binary_heap_destroy(heap);
            return false;
        }

        out_node = node_void;
        settled[out_node->index] = true;

        if (!graph_view_neighbors(graph, out_node, heap_push_neighbors, &(DijkstraHeapContext){ .heap = heap, .parents = parents, .distances = distances })) {
            free(settled);
            free(distances);
            free(parents);
            binary_heap_destroy(heap);
            return false;
        }
    }

    *out_distances = distances;
    *out_parents = parents;

    free(settled);
    free(distances);
    free(parents);
    binary_heap_destroy(heap);

    return true;
}
