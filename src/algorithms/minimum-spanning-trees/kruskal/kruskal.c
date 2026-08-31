// Declares the public Kruskal API.
#include "kruskal.h"
// Provides component tracking for cycle detection.
#include "../../../data-structures/graphs/disjoint-sets/union-find/union_find.h"
#include "../../sorting/comparison/quick-sort/quick_sort.h"
// Declares realloc and free.
#include <stdlib.h>

// Shares the dynamically collected candidate-edge array with one neighbor callback.
struct KruskalEdgeFinderContext {
    // Points to the number of initialized candidate-edge entries.
    size_t *unaccepted_edges_count;
    // Points to the allocated candidate-edge slot count.
    size_t *unaccepted_edges_capacity;
    // Identifies the Node whose outgoing neighbors are being enumerated.
    Node *from;
    // Points to the caller-owned candidate-edge array pointer.
    KruskalEdge **unaccepted_edges;
};

// Collects one canonical undirected edge from GraphView neighbor enumeration.
bool kruskal_edge_finder(Node* node, uint64_t weight, void *context) {
    // Rejects an invalid neighbor reported by a graph adapter.
    if (node == NULL) {
        return false;
    }

    // Restores the candidate-edge collection shared by this enumeration pass.
    KruskalEdgeFinderContext *kef_context = context;

    // Skips the mirrored edge and self-loops, keeping only lower-to-higher indexes.
    if (kef_context->from->index < node->index) {
        // Grows the candidate array before writing when every slot is occupied.
        if (*kef_context->unaccepted_edges_count == *kef_context->unaccepted_edges_capacity) {
            // Rejects a doubling operation that would overflow size_t.
            if (*kef_context->unaccepted_edges_capacity > SIZE_MAX / 2U) {
                return false;
            }

            // Allocates the first slot or doubles the existing candidate capacity.
            size_t new_capacity = *kef_context->unaccepted_edges_capacity == 0U ? 1U : *kef_context->unaccepted_edges_capacity * 2;
            // Rejects a byte-size calculation that would overflow size_t.
            if (new_capacity > SIZE_MAX / sizeof(KruskalEdge)) {
                return false;
            }

            // Requests larger contiguous storage without losing the old array on failure.
            KruskalEdge *edge = realloc(*kef_context->unaccepted_edges, sizeof(KruskalEdge) * new_capacity);
            if (edge == NULL) {
                return false;
            }
            
            // Publishes the successfully grown array and its new slot capacity.
            *kef_context->unaccepted_edges = edge;
            *kef_context->unaccepted_edges_capacity = new_capacity;
        }

        // Appends the one unique weighted edge represented by this callback.
        (*kef_context->unaccepted_edges)[*kef_context->unaccepted_edges_count] = (KruskalEdge) {
            .from = kef_context->from,
            .to = node,
            .weight = weight
        };

        // Expands the initialized range after the candidate edge is stored.
        (*kef_context->unaccepted_edges_count)++;
    }

    // Continues GraphView neighbor enumeration after processing this edge.
    return true;
}

// Orders candidate-edge pointers by nondecreasing weight for Kruskal selection.
int kruskal_quick_sort_compare(const void *left, const void *right) {
    // Restores the two candidate edges from quicksort's generic item pointers.
    const KruskalEdge *left_edge = left;
    const KruskalEdge *right_edge = right;

    // Places the lighter edge before the heavier edge.
    if (left_edge->weight < right_edge->weight) {
        return -1;
    }
    if (left_edge->weight > right_edge->weight) {
        return 1;
    }

    // Allows any order among equal-weight edges without changing optimal cost.
    return 0;
}

// Builds a minimum spanning forest from an undirected weighted GraphView.
bool kruskal_minimum_spanning_forest(const GraphView *graph, KruskalEdge *out_edges, size_t out_edge_capacity, size_t *out_edge_count, uint64_t *out_total_weight) {
    // Rejects a missing or incomplete graph adapter.
    if (!graph_view_is_valid(graph)) {
        return false;
    }

    // Rejects directed graphs because mirrored-edge suppression needs undirected input.
    if (graph_view_is_directed(graph)) {
        return false;
    }

    // Requires caller-owned locations for the completed forest metadata.
    if (out_edge_count == NULL) {
        return false;
    }

    // Requires caller-owned storage for the selected forest's total weight.
    if (out_total_weight == NULL) {
        return false;
    }

    // Reads the dense Node count used for GraphView enumeration.
    size_t count = graph_view_vertex_count(graph);

    // Returns the unique empty forest without requiring output-edge storage.
    if (count == 0U || count == 1U) {
        *out_edge_count = 0U;
        *out_total_weight = 0U;
        return true;
    }

    // Requires edge storage whenever a graph could produce selected edges.
    if (out_edges == NULL) {
        return false;
    }

    // Creates one disjoint-set component for every graph Node.
    UnionFind *union_find = union_find_create(count);
    if (union_find == NULL) {
        return false;
    }

    // Starts an empty dynamically grown collection of unique input edges.
    KruskalEdge *unaccepted_edges = NULL;
    size_t unaccepted_edges_count = 0U;
    size_t unaccepted_edges_capacity = 0U;

    // Enumerates every graph Node to collect its canonical outgoing edges.
    Node *out_node = NULL;
    size_t n = 0U;
    while(n < count) {
        // Retrieves the graph-owned Node at this dense index.
        if(!graph_view_node_at(graph, n, &out_node)) {
            union_find_destroy(union_find);
            free(unaccepted_edges);
            return false;
        }

        // Collects each undirected edge once through the canonical edge callback.
        if(!graph_view_neighbors(graph, out_node, kruskal_edge_finder, &(KruskalEdgeFinderContext){ .unaccepted_edges = &unaccepted_edges, .from = out_node, .unaccepted_edges_count = &unaccepted_edges_count, .unaccepted_edges_capacity = &unaccepted_edges_capacity })) {
            union_find_destroy(union_find);
            free(unaccepted_edges);
            return false;
        }

        n++;
    }

    // Reports an all-isolated forest after releasing unneeded search state.
    if (unaccepted_edges_count == 0U) {
        *out_edge_count = 0U;
        *out_total_weight = 0U;
        union_find_destroy(union_find);
        free(unaccepted_edges);
        return true;
    }

    if (unaccepted_edges_count > SIZE_MAX / sizeof(void *)) {
        union_find_destroy(union_find);
        free(unaccepted_edges);
        return false;
    }

    // Allocates sortable pointers while retaining candidate edges in stable storage.
    void **edge_pointers = malloc(sizeof(void *) * unaccepted_edges_count);
    if (edge_pointers == NULL) {
        union_find_destroy(union_find);
        free(unaccepted_edges);
        return false;
    }

    // Points every sortable slot at one collected candidate edge.
    n = 0U;
    while(n < unaccepted_edges_count) {
        edge_pointers[n] = &unaccepted_edges[n];
        n++;
    }

    // Orders candidate pointers from the lightest edge to the heaviest edge.
    if (!quick_sort(edge_pointers, unaccepted_edges_count, kruskal_quick_sort_compare)) {
        union_find_destroy(union_find);
        free(unaccepted_edges);
        free(edge_pointers);
        return false;
    }
    
    // Compacts accepted forest-edge pointers into the front of edge_pointers.
    size_t accepted_edge_count = 0U;
    // Accumulates the selected forest's weight before exposing caller output.
    uint64_t total_weight = 0;
    // Receives whether one union operation merged separate components.
    bool out_merged = false;
    n = 0U;
    while(n < unaccepted_edges_count) {
        // Restores the next lightest candidate edge.
        KruskalEdge *edge = edge_pointers[n];
        out_merged = false;

        // Accepts this edge only when it joins two formerly separate components.
        if (!union_find_union(union_find, edge->from->index, edge->to->index, &out_merged)) {
            union_find_destroy(union_find);
            free(unaccepted_edges);
            free(edge_pointers);
            return false;
        }

        if (out_merged) {
            // Retains the accepted edge without overwriting an unprocessed pointer.
            edge_pointers[accepted_edge_count] = edge;
            accepted_edge_count++;

            if (edge->weight > UINT64_MAX - total_weight) {
                union_find_destroy(union_find);
                free(unaccepted_edges);
                free(edge_pointers);
                return false;
            }

            total_weight = total_weight + edge->weight;
        }

        n++;
    }

    // Rejects insufficient caller storage before any selected edge is copied.
    if (accepted_edge_count > out_edge_capacity) {
        union_find_destroy(union_find);
        free(unaccepted_edges);
        free(edge_pointers);
        return false;
    }

    // Copies selected edge values from compacted pointers into caller storage.
    n = 0U;
    while(n < accepted_edge_count) {
        KruskalEdge *edge = edge_pointers[n];
        out_edges[n] = *edge;
        n++;
    }

    // Publishes completed forest metadata only after every output edge is written.
    *out_total_weight = total_weight;
    *out_edge_count = accepted_edge_count;

    // Releases all algorithm-owned state while preserving graph-owned Nodes.
    union_find_destroy(union_find);
    free(unaccepted_edges);
    free(edge_pointers);

    return true;
}
