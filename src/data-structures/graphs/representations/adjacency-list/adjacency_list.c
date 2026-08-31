#include "adjacency_list.h"

// Declares malloc, realloc, and free.
#include <stdlib.h>

// Defines the fields hidden from callers of the public API.
struct AdjacencyList {
    // Records whether each public edge also stores its reverse edge.
    bool directed;
    // Counts graph-owned nodes currently stored in nodes.
    size_t node_count;
    // Counts allocated node-pointer slots.
    size_t node_capacity;
    // Counts logical public edges; undirected reverse storage does not double it.
    size_t edge_count;
    // Points to the growable array of graph-owned node pointers.
    Node **nodes;
};

// Allocates an empty dynamic adjacency-list graph.
AdjacencyList *adjacency_list_create(bool directed) {
    // Allocates the graph wrapper.
    AdjacencyList *graph = malloc(sizeof(AdjacencyList));

    // Reports allocation failure before accessing graph fields.
    if (graph == NULL) {
        return NULL;
    }

    // Initializes the graph with no node or edge storage.
    graph->directed = directed;
    graph->node_count = 0U;
    graph->node_capacity = 0U;
    graph->edge_count = 0U;
    graph->nodes = NULL;

    return graph;
}

// Frees every graph-owned edge, node, node-pointer array, and graph wrapper.
void adjacency_list_destroy(AdjacencyList *graph) {
    // Treats a missing graph as a no-op.
    if (graph == NULL) {
        return;
    }

    // Holds each graph-owned node while its edge allocations are released.
    Node *temp = NULL;
    size_t n = 0U;
    while(n < graph->node_count) {
        // Retrieves the next graph-owned node from the node-pointer array.
        temp = graph->nodes[n];
        size_t _n = 0U;
        // Frees every separately allocated edge pointer stored by this node.
        while(_n < temp->edge_count) {
            free(temp->edges[_n]);
            _n++;
        }

        // Frees the array that held the edge pointers, then the node itself.
        free(temp->edges);
        free(temp);
        n++;
    }

    // Frees the node-pointer array and the graph wrapper.
    free(graph->nodes);
    free(graph);

    return;
}

// Adds one graph-owned node with a caller-owned payload value.
bool adjacency_list_add_node(AdjacencyList *graph, void *value, Node **out_node) {
    // Rejects a missing graph or output location.
    if (graph == NULL) {
        return false;
    }

    if (out_node == NULL) {
        return false;
    }

    // Allocates one graph-owned node after capacity is available.
    Node *node = malloc(sizeof(Node));
    // Reports node allocation failure without adding a logical node.
    if (node == NULL) {
        return false;
    }

    // Grows the node-pointer array only when every slot is occupied.
    if (graph->node_count == graph->node_capacity) {
        // Rejects a capacity that cannot double without overflowing size_t.
        if (graph->node_capacity > SIZE_MAX / 2U) {
            free(node);
            return false;
        }

        // Uses one initial slot, then doubles capacity for later additions.
        size_t new_capacity = graph->node_capacity == 0U ? 1U : graph->node_capacity * 2U;
        // Rejects a node-pointer count whose byte allocation would overflow.
        if (new_capacity > SIZE_MAX / sizeof(*graph->nodes)) {
            free(node);
            return false;
        }

        // Resizes through a temporary pointer so failure preserves the old array.
        Node **nodes = realloc(graph->nodes, sizeof(*graph->nodes) * new_capacity);
        // Reports resize failure without changing graph fields.
        if (nodes == NULL) {
            free(node);
            return false;
        }

        // Publishes the successfully resized array and capacity.
        graph->nodes = nodes;
        graph->node_capacity = new_capacity;
    }

    // Initializes payload, index, ownership, and empty edge storage.
    node->value = value;
    node->index = graph->node_count;
    node->owner = graph;
    node->edge_count = 0U;
    node->edge_capacity = 0U;
    node->edges = NULL;
    
    // Stores the node in the next dense index slot.
    graph->nodes[graph->node_count] = node;
    // Includes the new node in the graph's logical node count.
    graph->node_count++;

    // Returns the opaque node pointer to the caller.
    *out_node = node;

    // Reports successful node insertion.
    return true;
}

// Returns a node's caller-owned payload pointer.
bool adjacency_list_node_value(const Node *node, void **out_value) {
    // Rejects a missing node or output location.
    if (node == NULL) {
        return false;
    }

    if (out_value == NULL) {
        return false;
    }

    // Copies the payload pointer without transferring ownership.
    *out_value = node->value;

    // Reports successful payload lookup.
    return true;
}

// Returns the graph-owned node at a dense internal index.
bool adjacency_list_node_at(const AdjacencyList *graph, size_t index, Node **out_node) {
    // Rejects a missing graph or output location.
    if (graph == NULL) {
        return false;
    }

    // Rejects an index outside the occupied node-pointer range.
    if (index >= graph->node_count) {
        return false;
    }

    if (out_node == NULL) {
        return false;
    }

    // Copies the opaque node pointer to the caller without transferring ownership.
    *out_node = graph->nodes[index];

    // Reports successful node lookup.
    return true;
}

// Adds or updates one stored edge and records enough state to undo it.
static bool add_directed_edge(Node *from, Node *to, uint64_t weight,
                              bool *out_inserted, uint64_t *out_previous_weight) {
    // Initializes rollback state for the new public edge attempt.
    *out_inserted = false;
    *out_previous_weight = 0U;

    // Finds an existing edge before allocating replacement storage.
    for (size_t index = 0U; index < from->edge_count; index++) {
        // Updates the matching edge without changing node or graph counts.
        if (from->edges[index]->target == to) {
            *out_previous_weight = from->edges[index]->weight;
            from->edges[index]->weight = weight;
            return true;
        }
    }

    // Allocates the separately owned edge record for a new connection.
    Edge *edge = malloc(sizeof(*edge));
    if (edge == NULL) {
        return false;
    }

    // Grows the node's edge-pointer array only when it has no free slot.
    if (from->edge_count == from->edge_capacity) {
        // Rejects a capacity that cannot double safely.
        if (from->edge_capacity > SIZE_MAX / 2U) {
            free(edge);
            return false;
        }

        // Uses one initial slot and doubles capacity after that.
        size_t new_capacity = from->edge_capacity == 0U ? 1U : from->edge_capacity * 2U;
        // Rejects an allocation whose byte size would overflow.
        if (new_capacity > SIZE_MAX / sizeof(*from->edges)) {
            free(edge);
            return false;
        }

        // Preserves the old allocation if resizing fails.
        Edge **edges = realloc(from->edges, sizeof(*from->edges) * new_capacity);
        if (edges == NULL) {
            free(edge);
            return false;
        }

        // Publishes the resized pointer array and its new capacity.
        from->edges = edges;
        from->edge_capacity = new_capacity;
    }

    // Initializes the record after all allocations have succeeded.
    edge->target = to;
    edge->weight = weight;
    // Appends the new edge and exposes it to normal neighbor iteration.
    from->edges[from->edge_count] = edge;
    from->edge_count++;
    // Tells the caller that rollback must remove this record.
    *out_inserted = true;
    return true;
}

// Restores the source node to its state before add_directed_edge succeeded.
static void rollback_directed_edge(Node *from, Node *to, bool inserted,
                                   uint64_t previous_weight) {
    // Searches the source node for the directed edge being undone.
    for (size_t index = 0U; index < from->edge_count; index++) {
        if (from->edges[index]->target != to) {
            continue;
        }

        // Restores the previous weight when the operation updated an edge.
        if (!inserted) {
            from->edges[index]->weight = previous_weight;
            return;
        }

        // Removes a newly appended edge without leaving a gap in the array.
        size_t last_index = from->edge_count - 1U;
        free(from->edges[index]);
        from->edges[index] = from->edges[last_index];
        from->edges[last_index] = NULL;
        from->edge_count--;
        return;
    }
}

// Adds one logical edge, storing its reverse direction when the graph is undirected.
bool adjacency_list_add_edge(AdjacencyList *graph, Node *from, Node *to, uint64_t weight) {
    // Rejects a missing graph or endpoint before reading ownership fields.
    if (graph == NULL) {
        return false;
    }

    if (from == NULL) {
        return false;
    }

    if (to == NULL) {
        return false;
    }

    if (from->owner != graph) {
        return false;
    }

    if (to->owner != graph) {
        return false;
    }

    // Adds or updates the requested direction before considering a reverse edge.
    bool forward_inserted = false;
    uint64_t forward_previous_weight = 0U;
    if (!add_directed_edge(from, to, weight, &forward_inserted, &forward_previous_weight)) {
        return false;
    }

    // Directed edges and self-loops require only one stored edge record.
    if (graph->directed || from == to) {
        if (forward_inserted) {
            graph->edge_count++;
        }
        return true;
    }

    // Adds or updates the matching reverse edge for an undirected graph.
    bool reverse_inserted = false;
    uint64_t reverse_previous_weight = 0U;
    if (!add_directed_edge(to, from, weight, &reverse_inserted, &reverse_previous_weight)) {
        // Restores the first direction so a failed public call changes nothing.
        rollback_directed_edge(from, to, forward_inserted, forward_previous_weight);
        return false;
    }

    // Counts two stored directions as one logical undirected edge.
    if (forward_inserted && reverse_inserted) {
        graph->edge_count++;
    }

    return true;
}

// Reports whether one graph-owned node has an edge to another.
bool adjacency_list_has_edge(const AdjacencyList *graph, const Node *from, const Node *to) {
    // Rejects a missing graph or node pointer.
    if (graph == NULL) {
        return false;
    }

    if (from == NULL) {
        return false;
    }

    if (to == NULL) {
        return false;
    }

    // Rejects nodes that belong to another graph.
    if (from->owner != graph) {
        return false;
    }

    if (to->owner != graph) {
        return false;
    }

    // Scans the source node's sparse outgoing edge collection.
    size_t n = 0U;
    while(n < from->edge_count) {
        if (from->edges[n]->target == to) {
            // Reports the matching target edge immediately.
            return true;
        }

        n++;
    }

    // Reports absence after scanning every outgoing edge.
    return false;
}

// Visits one node's weighted outgoing edges in stored order.
bool adjacency_list_neighbors(const AdjacencyList *graph, const Node *node, AdjacencyListVisitFn visit, void *context) {
    // Rejects a missing graph, node, or visitor callback.
    if (graph == NULL) {
        return false;
    }

    if (node == NULL) {
        return false;
    }

    if (visit == NULL) {
        return false;
    }

    // Rejects a node that belongs to another graph.
    if (node->owner != graph) {
        return false;
    }

    // Visits each separately allocated edge record.
    size_t n = 0U;
    while(n < node->edge_count) {
        if (!visit(node->edges[n]->target, node->edges[n]->weight, context)) {
            // Propagates the visitor's request to stop iteration.
            return false;
        }

        n++;
    }

    // Reports successful full neighbor iteration.
    return true;
}

// Reports the number of graph-owned nodes.
size_t adjacency_list_node_count(const AdjacencyList *graph) {
    // Treats a missing graph as containing zero nodes.
    if (graph == NULL) {
        return 0U;
    }

    return graph->node_count;
}

// Reports the logical number of public edges.
size_t adjacency_list_edge_count(const AdjacencyList *graph) {
    // Treats a missing graph as containing zero edges.
    if (graph == NULL) {
        return 0U;
    }

    return graph->edge_count;
}

// Reports the concrete graph's node count through the GraphView callback type.
static size_t graph_view_compatible_vertex_count(const void *graph_context) {
    // Treats a missing context as an empty graph.
    if (graph_context == NULL) {
        return 0U;
    }

    // Restores the concrete type erased by the generic GraphView context.
    const AdjacencyList *graph = graph_context;

    // Reuses the representation's public count operation.
    return adjacency_list_node_count(graph);
}

// Adapts dense Node lookup to the GraphView callback type.
static bool graph_view_compatible_node_at(const void *graph_context, size_t index,
                                          Node **out_node) {
    const AdjacencyList *graph = graph_context;

    return adjacency_list_node_at(graph, index, out_node);
}

// Reuses concrete neighbor iteration through the GraphView callback type.
static bool graph_view_compatible_neighbors(const void *graph_context, const Node *node,
                                             GraphViewVisitFn visit, void *context) {
    // Rejects a missing GraphView backing context.
    if (graph_context == NULL) {
        return false;
    }

    // Restores the concrete type erased by the generic GraphView context.
    const AdjacencyList *graph = graph_context;

    // Lets the concrete API validate node ownership and visit each neighbor.
    return adjacency_list_neighbors(graph, node, visit, context);
}

static bool adjacency_list_is_directed(const void *context) {
    const AdjacencyList *graph = context;

    return graph->directed;
}

// Fills a non-owning GraphView that exposes this adjacency-list graph.
bool adjacency_list_graph_view(const AdjacencyList *graph, GraphView *out_view) {
    // Rejects missing graph or output storage before adapter construction.
    if (graph == NULL) {
        return false;
    }
        
    if (out_view == NULL) {
        return false;
    }

    out_view->context = graph;
    out_view->neighbors = graph_view_compatible_neighbors;
    out_view->vertex_count = graph_view_compatible_vertex_count;
    out_view->node_at = graph_view_compatible_node_at;
    out_view->is_directed = adjacency_list_is_directed;

    return true;
}
