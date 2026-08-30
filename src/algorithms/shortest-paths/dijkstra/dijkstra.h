#include "../../../data-structures/graphs/graph-view/graph_view.h"

#define DIJKSTRA_INFINITY UINT64_MAX

typedef struct GraphView GraphView;
typedef struct DijkstraNode DijkstraNode;
typedef struct DijkstraHeapContext DijkstraHeapContext;

bool dijkstra(const GraphView *graph, Node *source, uint64_t *out_distances, size_t *out_parents);
