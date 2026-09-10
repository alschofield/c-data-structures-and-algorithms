// Declares GraphView, bool, and size_t.
#include "../../../data-structures/graphs/graph-view/graph_view.h"

// Visits one discovered dense node index and returns false to stop traversal.
typedef bool (*DepthFirstSearchVisitFn)(size_t node_index, void *context);

// Names private LIFO frontier state used by neighbor enumeration.
typedef struct DepthFirstSearchStackContext DepthFirstSearchStackContext;

// Traverses indexes reachable from source in depth-first discovery order.
bool depth_first_search(const GraphView *graph, size_t source,
                        DepthFirstSearchVisitFn visit, void *context);
