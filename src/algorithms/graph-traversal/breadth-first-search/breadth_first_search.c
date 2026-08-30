// Declares the public BFS API.
#include "breadth_first_search.h"
// Declares malloc and free.
#include <stdlib.h>
#include "../../../data-structures/linear/queues/queue/queue.h"

// Stores BFS frontier and discovered-state shared by neighbor enumeration.
struct BreadthFirstSearchQueueContext {
    // Owns the FIFO frontier of discovered Nodes awaiting traversal.
    Queue *queue;
    // Marks dense Node indexes already enqueued into the frontier.
    bool *queued;
};

// Marks and queues one unvisited neighbor discovered through GraphView.
bool queue_enqueue_neighbor(Node *node, uint64_t weight, void *context) {
    // Rejects an invalid neighbor before reading its dense index.
    if (node == NULL) {
        return false;
    }

    // BFS ignores edge weights while preserving GraphView callback compatibility.
    (void)(weight);

    // Restores the queue and discovered-state context for this neighbor visit.
    BreadthFirstSearchQueueContext *context_ = context;

    // Enqueues every reachable Node at most once, including on cyclic graphs.
    if (!context_->queued[node->index]) {
        // Marks discovery before enqueue so alternate edges cannot duplicate it.
        context_->queued[node->index] = true;
        // Appends the discovered Node to the FIFO BFS frontier.
        if (!queue_enqueue(context_->queue, node)) {
            return false;
        }
    }

    // Allows GraphView to continue enumerating remaining neighbors.
    return true;
}

// Traverses Nodes reachable from source in FIFO breadth-first order.
bool breadth_first_search(const GraphView *graph, Node *source, BreadthFirstSearchVisitFn visit, void *context) {
    // Rejects a missing or incomplete graph adapter.
    if (!graph_view_is_valid(graph)) {
        return false;
    }

    // Rejects a missing caller-provided visit action.
    if (visit == NULL) {
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

    // Verifies that source is the graph-owned Node at its claimed dense index.
    Node *out_node = NULL;

    if (!graph_view_node_at(graph, source->index, &out_node)) {
        return false;
    }

    // Rejects a foreign Node that happens to have an in-range index.
    if (out_node != source) {
        return false;
    }

    // Allocates dense discovered-state tracking for cycle-safe traversal.
    bool *queued = malloc(sizeof(bool) * count);
    if (queued == NULL) {
        return false;
    }

    // Initializes every dense Node as not yet enqueued.
    size_t n = 0U;
    while(n < count) {
        queued[n] = false;
        n++;
    }

    // Creates the FIFO frontier used to preserve breadth-first order.
    Queue *queue = queue_create();
    if (queue == NULL) {
        free(queued);
        return false;
    }

    // Seeds BFS with the source Node.
    if (!queue_enqueue(queue, source)) {
        free(queued);
        queue_destroy(queue);
        return false;
    }

    // Marks source immediately so cyclic edges cannot re-enqueue it.
    queued[source->index] = true;

    // Receives each generic queue item before treating it as a Node pointer.
    void *node_void = NULL;

    // Processes the frontier until every reachable Node is visited or stopped.
    while(!queue_is_empty(queue)) {
        // Dequeues the earliest discovered Node from the FIFO frontier.
        if (!queue_dequeue(queue, &node_void)) {
            free(queued);
            queue_destroy(queue);
            return false;
        }

        // Restores the graph-specific Node pointer from the generic queue item.
        Node *node = node_void;

        // Lets caller state record this Node or request early traversal stop.
        if (!visit(node, context)) {
            free(queued);
            queue_destroy(queue);
            return false;
        }

        // Discovers and queues every unvisited outgoing neighbor of this Node.
        if (!graph_view_neighbors(graph, node, queue_enqueue_neighbor, &(BreadthFirstSearchQueueContext){ .queue = queue, .queued = queued })) {
            free(queued);
            queue_destroy(queue);
            return false;
        }

    }

    // Releases traversal-only state after the reachable frontier is exhausted.
    free(queued);
    queue_destroy(queue);

    // Reports successful full traversal.
    return true;
}
