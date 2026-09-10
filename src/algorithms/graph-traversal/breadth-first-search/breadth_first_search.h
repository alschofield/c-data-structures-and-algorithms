// Declares GraphView, bool, and size_t.
#include "../../../data-structures/graphs/graph-view/graph_view.h"

// Visits one discovered dense node index and returns false to stop traversal.
typedef bool (*BreadthFirstSearchVisitFn)(size_t node_index, void *context);

// Names private FIFO frontier state used by neighbor enumeration.
typedef struct BreadthFirstSearchQueueContext BreadthFirstSearchQueueContext;

// Traverses indexes reachable from source in breadth-first discovery order.
bool breadth_first_search(const GraphView *graph, size_t source,
                          BreadthFirstSearchVisitFn visit, void *context);
