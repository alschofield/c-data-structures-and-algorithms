// Declares the public Kruskal API.
#include "kruskal.h"
// Provides component tracking for cycle detection.
#include "../../../data-structures/graphs/disjoint-sets/union-find/union_find.h"
// Provides candidate edge ordering.
#include "../../sorting/comparison/quick-sort/quick_sort.h"
// Declares realloc and free.
#include <stdlib.h>

// Shares dynamically collected candidate edges with one neighbor callback.
struct KruskalEdgeFinderContext {
    KruskalEdge **edges;
    size_t *count;
    size_t *capacity;
    size_t from;
};

// Collects one canonical undirected edge from GraphView neighbor enumeration.
static bool kruskal_edge_finder(size_t to, uint64_t weight, void *context) {
    KruskalEdgeFinderContext *edge_context = context;
    if (edge_context->from >= to) return true;

    if (*edge_context->count == *edge_context->capacity) {
        if (*edge_context->capacity > SIZE_MAX / 2U) return false;
        size_t capacity = *edge_context->capacity == 0U ? 1U :
            *edge_context->capacity * 2U;
        if (capacity > SIZE_MAX / sizeof(**edge_context->edges)) return false;

        KruskalEdge *edges = realloc(
            *edge_context->edges,
            sizeof(**edge_context->edges) * capacity
        );
        if (edges == NULL) return false;
        *edge_context->edges = edges;
        *edge_context->capacity = capacity;
    }

    (*edge_context->edges)[*edge_context->count] = (KruskalEdge) {
        .from = edge_context->from,
        .to = to,
        .weight = weight
    };
    (*edge_context->count)++;
    return true;
}

// Orders candidate-edge pointers by nondecreasing weight and endpoint indexes.
static int kruskal_quick_sort_compare(const void *left, const void *right) {
    const KruskalEdge *left_edge = left;
    const KruskalEdge *right_edge = right;

    if (left_edge->weight < right_edge->weight) return -1;
    if (left_edge->weight > right_edge->weight) return 1;
    if (left_edge->from < right_edge->from) return -1;
    if (left_edge->from > right_edge->from) return 1;
    if (left_edge->to < right_edge->to) return -1;
    if (left_edge->to > right_edge->to) return 1;
    return 0;
}

// Builds a minimum spanning forest from an undirected weighted GraphView.
bool kruskal_minimum_spanning_forest(const GraphView *graph,
                                     KruskalEdge *out_edges,
                                     size_t out_edge_capacity,
                                     size_t *out_edge_count,
                                     uint64_t *out_total_weight) {
    if (!graph_view_is_valid(graph) || out_edge_count == NULL ||
        out_total_weight == NULL || graph_view_is_directed(graph)) return false;

    size_t vertex_count = graph_view_vertex_count(graph);
    if (vertex_count <= 1U) {
        *out_edge_count = 0U;
        *out_total_weight = 0U;
        return true;
    }
    if (out_edges == NULL) return false;

    UnionFind *sets = union_find_create(vertex_count);
    if (sets == NULL) return false;

    KruskalEdge *candidates = NULL;
    size_t candidate_count = 0U;
    size_t candidate_capacity = 0U;
    for (size_t from = 0U; from < vertex_count; from++) {
        if (!graph_view_node_at(graph, from) || !graph_view_neighbors(
                graph,
                from,
                kruskal_edge_finder,
                &(KruskalEdgeFinderContext) {
                    .edges = &candidates,
                    .count = &candidate_count,
                    .capacity = &candidate_capacity,
                    .from = from
                }
            )) {
            union_find_destroy(sets);
            free(candidates);
            return false;
        }
    }

    if (candidate_count == 0U) {
        *out_edge_count = 0U;
        *out_total_weight = 0U;
        union_find_destroy(sets);
        free(candidates);
        return true;
    }
    if (candidate_count > SIZE_MAX / sizeof(void *)) {
        union_find_destroy(sets);
        free(candidates);
        return false;
    }

    void **sorted = malloc(sizeof(*sorted) * candidate_count);
    if (sorted == NULL) {
        union_find_destroy(sets);
        free(candidates);
        return false;
    }
    for (size_t index = 0U; index < candidate_count; index++) {
        sorted[index] = &candidates[index];
    }
    if (!quick_sort(sorted, candidate_count, kruskal_quick_sort_compare)) {
        union_find_destroy(sets);
        free(candidates);
        free(sorted);
        return false;
    }

    size_t selected_count = 0U;
    uint64_t total_weight = 0U;
    for (size_t index = 0U; index < candidate_count; index++) {
        KruskalEdge *edge = sorted[index];
        bool merged = false;
        if (!union_find_union(sets, edge->from, edge->to, &merged)) {
            union_find_destroy(sets);
            free(candidates);
            free(sorted);
            return false;
        }
        if (merged) {
            if (edge->weight > UINT64_MAX - total_weight) {
                union_find_destroy(sets);
                free(candidates);
                free(sorted);
                return false;
            }
            sorted[selected_count++] = edge;
            total_weight += edge->weight;
        }
    }

    if (selected_count > out_edge_capacity) {
        union_find_destroy(sets);
        free(candidates);
        free(sorted);
        return false;
    }
    for (size_t index = 0U; index < selected_count; index++) {
        out_edges[index] = *(KruskalEdge *)sorted[index];
    }
    *out_edge_count = selected_count;
    *out_total_weight = total_weight;

    union_find_destroy(sets);
    free(candidates);
    free(sorted);
    return true;
}
