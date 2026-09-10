#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

// Declares size_t.
#include <stddef.h>
// Declares uint64_t.
#include <stdint.h>

// Names the graph-native node and edge types used by adjacency representations.
typedef struct Node Node;
typedef struct Edge Edge;

// Defines one weighted edge to a graph-native Node.
struct Edge {
    Node *target;
    uint64_t weight;
};

// Defines one graph-native Node owned by an adjacency representation.
struct Node {
    void *value;
    size_t index;
    void *owner;
    size_t edge_count;
    size_t edge_capacity;
    Edge **edges;
};

#endif
