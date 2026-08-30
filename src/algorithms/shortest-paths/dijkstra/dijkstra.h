// Declares GraphView, Node, uint64_t, and size_t.
#include "../../../data-structures/graphs/graph-view/graph_view.h"

// Marks an unreachable Node distance.
#define DIJKSTRA_INFINITY UINT64_MAX

// Names the private heap-entry type used by the implementation.
typedef struct DijkstraNode DijkstraNode;
// Names the private neighbor-relaxation context type.
typedef struct DijkstraHeapContext DijkstraHeapContext;

// Computes shortest nonnegative-weight paths from source into caller arrays.
bool dijkstra(const GraphView *graph, Node *source, uint64_t *out_distances, size_t *out_parents);
