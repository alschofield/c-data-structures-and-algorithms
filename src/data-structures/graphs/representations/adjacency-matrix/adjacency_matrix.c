// Declares malloc, realloc, and free.
#include <stdlib.h>
// Declares the public adjacency-matrix API.
#include "adjacency_matrix.h"

// Defines the fields hidden from callers of the public API.
struct AdjacencyMatrix {
    // Records whether logical edges have one or two stored directions.
    bool directed;
    // Counts logical public edges, not mirrored undirected storage.
    size_t edge_count;
    // Counts allocated node-pointer slots and flat matrix width.
    size_t node_capacity;
    // Counts occupied nodes and the active matrix dimension.
    size_t node_count;
    // Points to graph-owned nodes with stable dense indexes.
    Node **nodes;
    // Points to the flat edge matrix indexed by source and target node indexes.
    Edge *edges;
};

// Allocates an empty adjacency-matrix graph.
AdjacencyMatrix *adjacency_matrix_create(bool directed) {
    // Allocates the graph wrapper before initializing its fields.
    AdjacencyMatrix *matrix = malloc(sizeof(AdjacencyMatrix));
    // Reports allocation failure before accessing the wrapper.
    if (matrix == NULL) {
        return NULL;
    }

    // Records the directed-edge policy for later edge operations.
    matrix->directed = directed;
    // Starts with no logical edges.
    matrix->edge_count = 0U;
    // Starts with no node or matrix capacity.
    matrix->node_capacity = 0U;
    // Starts with an empty active matrix dimension.
    matrix->node_count = 0U;
    // Starts without node-pointer storage.
    matrix->nodes = NULL;
    matrix->edges = NULL;

    // Returns the initialized empty graph.
    return matrix;
}

// Frees all graph-owned matrix and node allocations.
void adjacency_matrix_destroy(AdjacencyMatrix *graph) {
    // Treats a missing graph as a no-op.
    if (graph == NULL) {
        return;
    }

    // Releases every present and absent Edge slot in the flat matrix allocation.
    free(graph->edges);

    // Visits only the occupied node-pointer slots, not spare capacity.
    size_t n = 0U;
    while(n < graph->node_count) {
        // Releases the graph-owned Node without freeing its caller-owned value.
        free(graph->nodes[n]);
        n++;
    }

    // Releases the allocation that held graph-owned Node pointers.
    free(graph->nodes);
    // Releases the graph wrapper after all owned allocations are gone.
    free(graph);

    // Ends the no-op destruction operation.
    return;
}

// Adds one graph-owned node and grows flat matrix capacity when necessary.
bool adjacency_matrix_add_node(AdjacencyMatrix *graph, void *value, Node **out_node) {
    // Rejects a missing graph before reading its fields.
    if (graph == NULL) {
        return false;
    }

    // Rejects a missing output location for the new node handle.
    if (out_node == NULL) {
        return false;
    }

    // Allocates the graph-owned node before modifying graph storage.
    Node *node = malloc(sizeof(Node));
    // Reports allocation failure without changing graph state.
    if (node == NULL) {
        return false;
    }

    // Grows node-pointer capacity when every slot is occupied.
    if (graph->node_count == graph->node_capacity) {
        // Rejects a capacity that cannot double safely.
        if (graph->node_capacity > SIZE_MAX / 2U) {
            free(node);
            return false;
        }

        // Uses one initial slot, then doubles future capacities.
        size_t new_capacity = graph->node_capacity == 0U ? 1U : graph->node_capacity * 2U;
        // Rejects a node-pointer allocation whose byte size would overflow.
        if (new_capacity > SIZE_MAX / sizeof(*graph->nodes)) {
            free(node);
            return false;
        }

        // Rejects a capacity whose squared cell count would overflow.
        if (new_capacity > SIZE_MAX / new_capacity) {
            free(node);
            return false;
        }

        size_t new_edge_capacity = new_capacity * new_capacity;
        // Rejects a flat Edge allocation whose byte size would overflow.
        if (new_edge_capacity > SIZE_MAX / sizeof(Edge)) {
            free(node);
            return false;
        }

        // Allocates a candidate node-pointer array while leaving graph state intact.
        Node **nodes = malloc(sizeof(*graph->nodes) * new_capacity);
        // Reports node-pointer allocation failure before the graph fields are updated.
        if (nodes == NULL) {
            free(node);
            return false;
        }

        // Allocates a candidate Edge matrix before releasing the old allocation.
        Edge *edges = malloc(sizeof(*graph->edges) * (new_edge_capacity));
        // Discards the candidate node array when the Edge allocation fails.
        if (edges == NULL) {
            free(node);
            free(nodes);
            return false;
        }

        // Copies every occupied Node pointer into the candidate pointer array.
        size_t n = 0U;
        while(n < graph->node_count) {
            nodes[n] = graph->nodes[n];
            n++;
        }

        // Initializes every new matrix slot as an absent Edge.
        n = 0U;
        while(n < new_edge_capacity) {
            edges[n].target = NULL;
            edges[n].weight = 0U;
            n++;
        }

        // Scans old matrix cells and remaps each present Edge to its new row width.
        n = 0U;
        size_t old_edge_capacity = graph->node_capacity * graph->node_capacity;
        while(n < old_edge_capacity) {
            Edge edge = graph->edges[n];
            if (edge.target != NULL) {
                // Recovers the old source row and target column from the flat offset.
                size_t from_index = n / graph->node_capacity;
                size_t to_index = n % graph->node_capacity;
                // Stores the Edge at the same coordinates under the new row width.
                edges[from_index * new_capacity + to_index] = edge;
            }

            n++;
        }

        // Releases the replaced allocations only after both candidates are ready.
        free(graph->nodes);
        free(graph->edges);
        // Publishes the fully prepared replacement allocations.
        graph->nodes = nodes;
        graph->edges = edges;
        // Records the capacity shared by node pointers and flat matrix width.
        graph->node_capacity = new_capacity;
    }

    // Stores the caller-owned payload without taking ownership.
    node->value = value;
    // Records the graph used to reject foreign node handles.
    node->owner = graph;
    // Assigns the next stable dense row and column index.
    node->index = graph->node_count;
    // Leaves shared adjacency-list edge fields unused by this representation.

    // Stores the new graph-owned node at its dense index.
    graph->nodes[graph->node_count] = node;
    // Includes the new node in the active matrix dimension.
    graph->node_count++;

    // Returns the opaque node handle without transferring graph ownership.
    *out_node = node;

    // Reports successful node insertion.
    return true;
}

// Returns one node's caller-owned payload pointer.
bool adjacency_matrix_node_value(const Node *node, void **out_value) {
    // Rejects a missing node handle.
    if (node == NULL) {
        return false;
    }

    // Rejects a missing payload output location.
    if (out_value == NULL) {
        return false;
    }

    // Copies the payload pointer without transferring ownership.
    *out_value = node->value;

    return true;
}

// Returns the graph-owned node at one dense internal index.
bool adjacency_matrix_node_at(const AdjacencyMatrix *graph, size_t index, Node **out_node) {
    // Rejects a missing graph before reading its fields.
    if (graph == NULL) {
        return false;
    }

    // Rejects a missing node output location.
    if (out_node == NULL) {
        return false;
    }

    // Rejects indexes outside the active matrix dimension.
    if (index >= graph->node_count) {
        return false;
    }

    // Returns the graph-owned node without transferring ownership.
    *out_node = graph->nodes[index];

    // Reports successful node lookup.
    return true;
}

// Adds one logical edge and mirrors it when the graph is undirected.
bool adjacency_matrix_add_edge(AdjacencyMatrix *graph, Node *from, Node *to, uint64_t weight) {
    // Rejects a missing graph before reading its fields.
    if (graph == NULL) {
        return false;
    }

    // Rejects a missing source node.
    if (from == NULL) {
        return false;
    }

    // Rejects a source node owned by another graph.
    if (from->owner != graph) {
        return false;
    }

    // Rejects a missing target node.
    if (to == NULL) {
        return false;
    }

    // Rejects a target node owned by another graph.
    if (to->owner != graph) {
        return false;
    }

    // Calculates the direct source-row and target-column matrix slot.
    size_t edge_address = from->index * graph->node_capacity + to->index;
    // Rejects duplicate edges rather than changing their stored weight.
    if (graph->edges[edge_address].target != NULL) {
        return false;
    }

    // Stores the directed edge at its direct matrix address.
    graph->edges[edge_address].target = to;
    graph->edges[edge_address].weight = weight;

    // Stores the matching reverse direction for an undirected graph.
    if (!graph->directed) {
        edge_address = to->index * graph->node_capacity + from->index;
        graph->edges[edge_address].target = from;
        graph->edges[edge_address].weight = weight;
    }

    // Counts one public edge even when the graph stores its mirror.
    graph->edge_count++;

    // Reports successful edge insertion.
    return true;
}

// Removes one logical edge and its mirror when the graph is undirected.
bool adjacency_matrix_remove_edge(AdjacencyMatrix *graph, Node *from, Node *to) {
    // Rejects a missing graph before reading its fields.
    if (graph == NULL) {
        return false;
    }

    // Rejects a missing source node.
    if (from == NULL) {
        return false;
    }

    // Rejects a source node owned by another graph.
    if (from->owner != graph) {
        return false;
    }

    // Rejects a missing target node.
    if (to == NULL) {
        return false;
    }

    // Rejects a target node owned by another graph.
    if (to->owner != graph) {
        return false;
    }

    // Calculates the direct source-row and target-column matrix slot.
    size_t edge_address = from->index * graph->node_capacity + to->index;

    // Rejects removal of an absent edge.
    if (graph->edges[edge_address].target == NULL) {
        return false;
    }

    // Marks the directed matrix slot absent and clears its stale weight.
    graph->edges[edge_address].target = NULL;
    graph->edges[edge_address].weight = 0U;

    // Clears the matching reverse slot for an undirected graph.
    if (!graph->directed) {
        edge_address = to->index * graph->node_capacity + from->index;
        graph->edges[edge_address].target = NULL;
        graph->edges[edge_address].weight = 0U;
    }

    // Removes one logical public edge from the graph count.
    graph->edge_count--;

    // Reports successful edge removal.
    return true;
}

// Reports whether one graph-owned node has a direct edge to another.
bool adjacency_matrix_has_edge(const AdjacencyMatrix *graph, const Node *from, const Node *to) {
    // Rejects a missing graph before reading its fields.
    if (graph == NULL) {
        return false;
    }

    // Rejects a missing source node.
    if (from == NULL) {
        return false;
    }

    // Rejects a source node owned by another graph.
    if (from->owner != graph) {
        return false;
    }

    // Rejects a missing target node.
    if (to == NULL) {
        return false;
    }

    // Rejects a target node owned by another graph.
    if (to->owner != graph) {
        return false;
    }

    // Reads the direct source-row and target-column matrix slot.
    size_t edge_address = from->index * graph->node_capacity + to->index;
    Edge edge = graph->edges[edge_address];

    // Reports an absent slot without exposing its irrelevant weight.
    if (edge.target == NULL) {
        return false;
    }

    // Reports that the direct matrix slot contains an Edge.
    return true;
}

// Returns the weight stored for one direct graph-owned edge.
bool adjacency_matrix_get_edge_weight(const AdjacencyMatrix *graph, const Node *from, const Node *to, uint64_t *out_weight) {
    // Rejects a missing graph before reading its fields.
    if (graph == NULL) {
        return false;
    }

    // Rejects a missing source node.
    if (from == NULL) {
        return false;
    }

    // Rejects a source node owned by another graph.
    if (from->owner != graph) {
        return false;
    }

    // Rejects a missing target node.
    if (to == NULL) {
        return false;
    }

    // Rejects a target node owned by another graph.
    if (to->owner != graph) {
        return false;
    }

    // Rejects a missing caller-owned output location.
    if (out_weight == NULL) {
        return false;
    }

    // Reads the direct source-row and target-column matrix slot.
    size_t edge_address = from->index * graph->node_capacity + to->index;
    Edge edge = graph->edges[edge_address];

    // Rejects a lookup for an absent edge.
    if (edge.target == NULL) {
        return false;
    }

    // Copies the stored weight without transferring ownership.
    *out_weight = edge.weight;

    // Reports successful edge-weight lookup.
    return true;
}

// Visits one graph-owned node's weighted outgoing edges in target-index order.
bool adjacency_matrix_neighbors(const AdjacencyMatrix *graph, const Node *node, AdjacencyMatrixVisitFn visit, void *context) {
    // Rejects a missing graph before reading its fields.
    if (graph == NULL) {
        return false;
    }

    // Rejects a missing source node.
    if (node == NULL) {
        return false;
    }

    // Rejects a source node owned by another graph.
    if (node->owner != graph) {
        return false;
    }

    // Rejects a missing visitor callback.
    if (visit == NULL) {
        return false;
    }

    // Scans every active target column in the source node's flat matrix row.
    size_t n = 0U;
    while(n < graph->node_count) {
        // Calculates the direct row-and-column address for this possible edge.
        size_t edge_index = node->index * graph->node_capacity + n;
        // Skips an absent matrix slot without invoking the visitor.
        if (graph->edges[edge_index].target != NULL) {
            // Stops immediately when the visitor declines further neighbors.
            if (!visit(graph->edges[edge_index].target, graph->edges[edge_index].weight, context)) {
                return false;
            }
        }

        // Advances to the next target column.
        n++;
    }

    // Reports that every active target column was visited.
    return true;
}
// Reports the number of graph-owned nodes.
size_t adjacency_matrix_node_count(const AdjacencyMatrix *graph) {
    // Treats a missing graph as containing zero nodes.
    if (graph == NULL) {
        return false;
    }

    // Returns the active matrix dimension.
    return graph->node_count;
}

// Reports the number of logical public edges.
size_t adjacency_matrix_edge_count(const AdjacencyMatrix *graph) {
    // Treats a missing graph as containing zero edges.
    if (graph == NULL) {
        return 0U;
    }

    // Returns the count without exposing mirrored undirected storage.
    return graph->edge_count;
}

// Adapts an adjacency-matrix context to GraphView's node-count callback type.
static size_t adjacency_matrix_view_vertex_count(const void *context) {
    // Restores the concrete graph type erased by GraphView.
    const AdjacencyMatrix *graph = context;

    // Reuses the concrete node-count operation.
    return adjacency_matrix_node_count(graph);
}

// Adapts dense Node lookup to the GraphView callback type.
static bool adjacency_matrix_view_node_at(const void *context, size_t index,
                                          Node **out_node) {
    const AdjacencyMatrix *graph = context;

    return adjacency_matrix_node_at(graph, index, out_node);
}

// Adapts adjacency-matrix neighbor iteration to GraphView's callback type.
static bool adjacency_matrix_view_neighbors(const void *context, const Node *node, GraphViewVisitFn visit, void *visit_context) {
    // Restores the concrete graph type erased by GraphView.
    const AdjacencyMatrix *graph = context;

    // Delegates validation and neighbor scanning to the concrete operation.
    return adjacency_matrix_neighbors(graph, node, visit, visit_context);
}

// Fills a non-owning GraphView adapter for this adjacency-matrix graph.
bool adjacency_matrix_graph_view(const AdjacencyMatrix *graph, GraphView *out_view) {
    // Rejects a missing backing graph.
    if (graph == NULL) {
        return false;
    }

    // Rejects missing caller-provided adapter storage.
    if (out_view == NULL) {
        return false;
    }

    // Stores the concrete graph without transferring ownership.
    out_view->context = graph;
    // Supplies the representation-specific neighbor adapter.
    out_view->neighbors = adjacency_matrix_view_neighbors;
    // Supplies the representation-specific node-count adapter.
    out_view->vertex_count = adjacency_matrix_view_vertex_count;
    // Supplies the representation-specific dense Node lookup adapter.
    out_view->node_at = adjacency_matrix_view_node_at;

    // Reports successful GraphView construction.
    return true;
}
