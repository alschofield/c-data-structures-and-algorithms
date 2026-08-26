#include "adjacency_list.h"

// Declares malloc, realloc, and free.
#include <stdlib.h>

// Defines one outgoing weighted edge stored by a source node.
struct Edge {
    // Points to the neighboring target node without owning it.
    Node *target;
    // Stores the nonnegative traversal cost of this edge.
    uint64_t weight;
};

// Defines one graph-owned node with a caller-owned payload.
struct Node {
    // Points to caller-owned data; graph destruction never frees it.
    void *value;
    // Stores the stable dense index used by GraphView algorithms.
    size_t index;
    // Identifies the graph that owns this node for foreign-node validation.
    AdjacencyList *owner;
    // Counts occupied edge-array slots.
    size_t edge_count;
    // Counts allocated edge-array slots.
    size_t edge_capacity;
    // Points to the growable array of pointers to separately allocated edges.
    Edge **edges;
};

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

bool add_edge(AdjacencyList *graph, Node *from, Node *to, uint64_t weight, bool *inserted, uint64_t *previous_weight) {
    Edge *edge = malloc(sizeof(Edge));
    if (edge == NULL) {
        return false;
    }

    edge->target = to;
    edge->weight = weight;

    size_t n = 0U;
    while(n < from->edge_count) {
        if (from->edges[n]->target == to) {
            previous_weight = from->edges[n]->weight;
            inserted = false;
            free(from->edges[n]);
            from->edges[n] = edge;
            return true;
        }

        n++;
    }

    if (from->edge_count == from->edge_capacity) {
        if (from->edge_capacity > SIZE_MAX / 2U) {
            free(edge);
            return false;
        }

        size_t new_capacity = from->edge_count == 0U ? 1U : from->edge_count * 2;
        if (new_capacity > SIZE_MAX / sizeof(*from->edges)) {
            free(edge);
            return false;
        }

        Edge **edges = realloc(from->edges, sizeof(*from->edges) * new_capacity);
        if (edges == NULL) {
            free(edge);
            return false;
        }

        from->edges = edges;
    }

    from->edges[from->edge_count] = edge;
    from->edge_count++;
    inserted = true;

    return true;
}

bool rollback_edge(AdjacencyList *graph, Node *from, Node *to, bool *inserted, uint64_t *previous_weight) {
    size_t n = 0U;
    while(n < from->edge_count) {
        if (from->edges[n]->target == to) {
            if (inserted) {
                free(from->edges[n]);
                from->edges[n] = NULL;
            } else {
                from->edges[n]->weight = *previous_weight;
            }

            return true;
        }

        n++;
    }

    return false;
}

// TODO: Validate owners, grow edge storage, store weight, and mirror undirected edges.
bool adjacency_list_add_edge(AdjacencyList *graph, Node *from, Node *to, uint64_t weight) {
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

    bool inserted = false;
    uint64_t previous_weight = 0U;
    if (!graph->directed && from != to) {
        if(!add_edge(graph, to, from, weight, &inserted, &previous_weight)) {
            return false;
        }

        if(!add_edge(graph, from, to, weight, NULL, NULL)) {
            rollback_edge(graph, to, from, &inserted, &previous_weight);
            return false;
        } else {
            if (!inserted) {
                from->edge_count++;
                to->edge_count++;
                graph->edge_count++;
            }

            return true;
        }
    } else {
        if (add_edge(graph, from, to, weight, &inserted, &previous_weight)) {
            if (!inserted) {
                from->edge_count++;
                graph->edge_count++;
            }

            return true;
        } else {
            return false;
        }
    }
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

// TODO: Fill out_view with callbacks that expose this graph through GraphView.
bool adjacency_list_graph_view(const AdjacencyList *graph, GraphView *out_view) {
    // Rejects missing graph or output storage before adapter construction.
    if (graph == NULL) {
        return false;
    }
        
    if (out_view == NULL) {
        return false;
    }

    out_view->context = graph;
    out_view->neighbors = adjacency_list_neighbors;
    out_view->vertex_count = adjacency_list_node_count;

    return true;
}
