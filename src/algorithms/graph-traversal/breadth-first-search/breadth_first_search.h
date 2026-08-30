// Declares GraphView and shared Node handles.
#include "../../../data-structures/graphs/graph-view/graph_view.h"

// Visits one discovered Node and returns false to stop traversal early.
typedef bool (*BreadthFirstSearchVisitFn)(Node *node, void *context);

typedef struct BreadthFirstSearchQueueContext BreadthFirstSearchQueueContext;

// Traverses Nodes reachable from source in breadth-first discovery order.
bool breadth_first_search(const GraphView *graph, Node *source, BreadthFirstSearchVisitFn visit, void *context);
