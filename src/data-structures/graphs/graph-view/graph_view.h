#include <stddef.h>
#include <stdbool.h>

typedef struct GraphView GraphView;
typedef bool (*GraphViewVisitFn)(size_t neighbor, uint64_t weight, void *context);
typedef size_t (*GraphViewVertexCountFn)(const void *graph_context);
typedef bool (*GraphViewNeighborsFn)(const void *graph_context, size_t vertex, GraphViewVisitFn visit, void *context);

bool graph_view_is_valid(const GraphView *view);
size_t graph_view_vertex_count(const GraphView *view);
bool graph_view_neighbors(const GraphView *view, size_t vertex, GraphViewVisitFn visit, void *context);
