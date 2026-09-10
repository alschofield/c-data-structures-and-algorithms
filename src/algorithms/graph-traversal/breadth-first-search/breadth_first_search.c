// Declares the public BFS API.
#include "breadth_first_search.h"
// Provides the FIFO frontier implementation.
#include "../../../data-structures/linear/queues/queue/queue.h"
// Declares malloc and free.
#include <stdlib.h>

// Stores BFS frontier and discovered-state shared by neighbor enumeration.
struct BreadthFirstSearchQueueContext {
    // Owns the FIFO frontier of discovered indexes awaiting traversal.
    Queue *queue;
    // Marks dense indexes already enqueued into the frontier.
    bool *queued;
    // Stores stable index values whose addresses can be held by Queue.
    size_t *indexes;
};

// Marks and queues one unvisited neighbor discovered through GraphView.
static bool queue_enqueue_neighbor(size_t neighbor_index, uint64_t weight,
                                   void *context) {
    // BFS ignores edge weights while preserving GraphView callback compatibility.
    (void)weight;

    // Restores the queue and discovered-state context for this neighbor visit.
    BreadthFirstSearchQueueContext *bfs_context = context;

    // Enqueues every reachable index at most once, including on cyclic graphs.
    if (!bfs_context->queued[neighbor_index]) {
        bfs_context->queued[neighbor_index] = true;
        if (!queue_enqueue(bfs_context->queue, &bfs_context->indexes[neighbor_index])) {
            return false;
        }
    }

    return true;
}

// Traverses indexes reachable from source in FIFO breadth-first order.
bool breadth_first_search(const GraphView *graph, size_t source,
                          BreadthFirstSearchVisitFn visit, void *context) {
    if (!graph_view_is_valid(graph) || visit == NULL) {
        return false;
    }

    size_t count = graph_view_vertex_count(graph);
    if (count == 0U || source >= count || !graph_view_node_at(graph, source)) {
        return false;
    }

    bool *queued = malloc(sizeof(*queued) * count);
    size_t *indexes = malloc(sizeof(*indexes) * count);
    if (queued == NULL || indexes == NULL) {
        free(queued);
        free(indexes);
        return false;
    }

    for (size_t index = 0U; index < count; index++) {
        queued[index] = false;
        indexes[index] = index;
    }

    Queue *queue = queue_create();
    if (queue == NULL) {
        free(queued);
        free(indexes);
        return false;
    }

    if (!queue_enqueue(queue, &indexes[source])) {
        queue_destroy(queue);
        free(queued);
        free(indexes);
        return false;
    }
    queued[source] = true;

    while (!queue_is_empty(queue)) {
        void *entry = NULL;
        if (!queue_dequeue(queue, &entry)) {
            queue_destroy(queue);
            free(queued);
            free(indexes);
            return false;
        }

        size_t node_index = *(size_t *)entry;
        if (!visit(node_index, context) || !graph_view_neighbors(
                graph,
                node_index,
                queue_enqueue_neighbor,
                &(BreadthFirstSearchQueueContext) {
                    .queue = queue,
                    .queued = queued,
                    .indexes = indexes
                }
            )) {
            queue_destroy(queue);
            free(queued);
            free(indexes);
            return false;
        }
    }

    queue_destroy(queue);
    free(queued);
    free(indexes);
    return true;
}
