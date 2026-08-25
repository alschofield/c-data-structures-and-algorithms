#include <stdlib.h>
#include "adjacency_matrix.h"

struct Edge {
    Node *target;
    uint64_t weight;
};

struct Node {
    void *value;
    size_t index;
    AdjacencyMatrix *owner;
    size_t edge_count;
    size_t edge_capacity;
    Edge **edges;
};

struct AdjacencyMatrix {
    bool directed;
    size_t edge_count;
    size_t node_capacity;
    size_t node_count;
    Node **nodes;
};

AdjacencyMatrix *adjacency_matrix_create(bool directed) {
    AdjacencyMatrix *matrix = malloc(sizeof(AdjacencyMatrix));
    if (matrix == NULL) {
        return NULL;
    }

    matrix->directed = directed;
    matrix->edge_count = 0U;
    matrix->node_capacity = 0U;
    matrix->node_count = 0U;
    matrix->nodes = NULL;

    return matrix;
}

void adjacency_matrix_destroy(AdjacencyMatrix *graph);

bool adjacency_matrix_add_node(AdjacencyMatrix *graph, void *value, Node **out_node) {
    if (graph == NULL) {
        return false;
    }

    if (out_node == NULL) {
        return false;
    }

    Node *node = malloc(sizeof(Node));
    if (node == NULL) {
        return false;
    }

    if (graph->node_count == graph->node_capacity) {
        if (graph->node_capacity > SIZE_MAX / 2U) {
            free(node);
            return false;
        }

        size_t new_capacity = graph->node_capacity == 0U ? 1U : graph->node_capacity * 2U;
        if (new_capacity > SIZE_MAX / sizeof(*graph->nodes) * new_capacity) {
            free(node);
            return false;
        }

        Node **nodes = realloc(graph->nodes, sizeof(Node) * new_capacity);
        if (nodes == NULL) {
            free(node);
            return false;
        }

        graph->nodes = nodes;
        graph->node_capacity = new_capacity;
    }

    node->value = value;
    node->owner = graph;
    node->index = graph->node_count;
    node->edge_capacity = 0U;
    node->edge_count = 0U;
    node->edges = NULL;

    graph->nodes[graph->node_count] = node;
    graph->node_count++;

    *out_node = node;

    return true;
}

bool adjacency_matrix_node_value(const Node *node, void **out_value) {
    if (node == NULL) {
        return false;
    }

    if (out_value == NULL) {
        return false;
    }

    *out_value = node->value;

    return false;
}

bool adjacency_matrix_node_at(const AdjacencyMatrix *graph, size_t index, Node **out_node) {
    if (graph == NULL) {
        return false;
    }

    if (out_node == NULL) {
        return false;
    }

    if (index >= graph->node_count) {
        return false;
    }

    *out_node = graph->nodes[index];

    return true;
}

bool adjacency_matrix_add_edge(AdjacencyMatrix *graph, Node *from, Node *to, uint64_t weight);

bool adjacency_matrix_remove_edge(AdjacencyMatrix *graph, Node *from, Node *to);

bool adjacency_matrix_has_edge(const AdjacencyMatrix *graph, const Node *from, const Node *to);

bool adjacency_matrix_get_edge_weight(const AdjacencyMatrix *graph, const Node *from, const Node *to, uint64_t *out_weight);

bool adjacency_matrix_neighbors(const AdjacencyMatrix *graph, const Node *node, AdjacencyMatrixVisitFn visit, void *context);

size_t adjacency_matrix_node_count(const AdjacencyMatrix *graph) {
    if (graph == NULL) {
        return false;
    }

    return graph->node_count;
}

size_t adjacency_matrix_edge_count(const AdjacencyMatrix *graph) {
    if (graph == NULL) {
        return 0U;
    }

    return graph->edge_count;
}

bool adjacency_matrix_graph_view(const AdjacencyMatrix *graph, GraphView *out_view) {
    if (graph == NULL) {
        return false;
    }

    if (out_view == NULL) {
        return false;
    }

    out_view->context = graph;
    out_view->neighbors = adjacency_matrix_neighbors;
    out_view->vertex_count = adjacency_matrix_node_count;

    return true;
}
