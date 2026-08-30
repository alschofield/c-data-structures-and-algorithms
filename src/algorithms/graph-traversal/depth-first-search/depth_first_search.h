// Declares GraphView and shared Node handles.
#include "../../../data-structures/graphs/graph-view/graph_view.h"

// Visits one discovered Node and returns false to stop traversal early.
typedef bool (*DepthFirstSearchVisitFn)(Node *node, void *context);

typedef struct DepthFirstSearchStackContext DepthFirstSearchStackContext;

// Traverses Nodes reachable from source in depth-first discovery order.
bool depth_first_search(const GraphView *graph, Node *source, DepthFirstSearchVisitFn visit, void *context);
