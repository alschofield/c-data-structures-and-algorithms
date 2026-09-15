# Graph View

`GraphView` is a non-owning, read-only adapter from a concrete graph-like structure to dense vertex indexes and weighted outgoing-neighbor iteration.

## C API
```c
typedef bool (*GraphViewVisitFn)(size_t neighbor_index, uint64_t weight, void *context);
typedef size_t (*GraphViewVertexCountFn)(const void *graph_context); typedef bool (*GraphViewIsDirectedFn)(const void *graph_context);
typedef bool (*GraphViewNodeAtFn)(const void *graph_context, size_t index);
typedef bool (*GraphViewNeighborsFn)(const void *graph_context, size_t index, GraphViewVisitFn visit, void *context);
struct GraphView { const void *context; GraphViewVertexCountFn vertex_count; GraphViewNodeAtFn node_at; GraphViewNeighborsFn neighbors; GraphViewIsDirectedFn is_directed; };
bool graph_view_is_valid(const GraphView *view); size_t graph_view_vertex_count(const GraphView *view); bool graph_view_node_at(const GraphView *view, size_t index);
bool graph_view_neighbors(const GraphView *view, size_t index, GraphViewVisitFn visit, void *context); bool graph_view_is_directed(const GraphView *view);
```

## Behavior, Lifetime, and Invariants
- A valid view has non-null context and all four callbacks. Invalid views return false for boolean operations and zero for vertex count.
- `node_at` and `neighbors` reject indexes outside `[0, vertex_count)`; neighbor iteration also requires a callback and propagates a callback's false result.
- It allocates nothing and owns neither context nor callback state. The backing object and its callbacks must outlive every use; mutations must preserve the adapter's dense-index and neighbor contracts.

## Complexity and Verification
Wrapper overhead is O(1), plus backing callback cost. Verify with `make test NAME=data-structures/graphs/graph-view`.
