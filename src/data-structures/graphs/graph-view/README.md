# Graph View

Representation-independent read-only graph interface. A GraphView adapts an
adjacency list, adjacency matrix, imported graph, or future graph structure so
algorithms can enumerate weighted outgoing edges without knowing the backing
representation.

## How It Works

Graph algorithms need a node count, dense index lookup, and a way to visit one
index's outgoing neighbors and weights. GraphView packages those operations as
a context pointer plus function pointers. Dense `size_t` indexes are the
generic node handles: adjacency representations translate native Nodes to
indexes, while structural adapters traverse native links directly. Unweighted
structures expose every link with weight `1U`.

## Required API

```c
typedef struct GraphView GraphView;
typedef bool (*GraphViewVisitFn)(size_t neighbor_index, uint64_t weight,
                                 void *context);
typedef size_t (*GraphViewVertexCountFn)(const void *graph_context);
typedef bool (*GraphViewIsDirectedFn)(const void *graph_context);
typedef bool (*GraphViewNodeAtFn)(const void *graph_context, size_t index);
typedef bool (*GraphViewNeighborsFn)(const void *graph_context, size_t index,
                                      GraphViewVisitFn visit,
                                      void *context);

struct GraphView {
    const void *context;
    GraphViewVertexCountFn vertex_count;
    GraphViewNodeAtFn node_at;
    GraphViewNeighborsFn neighbors;
    GraphViewIsDirectedFn is_directed;
};

bool graph_view_is_valid(const GraphView *view);
size_t graph_view_vertex_count(const GraphView *view);
bool graph_view_is_directed(const GraphView *view);
bool graph_view_node_at(const GraphView *view, size_t index);
bool graph_view_neighbors(const GraphView *view, size_t index,
                           GraphViewVisitFn visit, void *context);
```

## Contract

- A valid view has non-`NULL` context, `vertex_count`, `node_at`, `neighbors`,
  and `is_directed` fields.
- Every adapter exposes dense indexes from zero through `vertex_count - 1`.
- `graph_view_node_at` validates native lookup of one dense index.
- `graph_view_neighbors` visits each outgoing edge exactly once, passing its
  target dense index and nonnegative `uint64_t` weight.
- A visitor returning `false` stops iteration and makes
  `graph_view_neighbors` return `false`.
- The view never owns, copies, mutates, or destroys its backing graph.
- Structural adapters must not cache or duplicate native links as graph edges.
- The backing structure must not mutate while a structural GraphView is used;
  mutation can change dense index meaning.
- Concrete graph representations expose a function that fills an output
  `GraphView`; imported graphs may construct the struct directly with adapter
  callbacks.

## Complexity Targets

- `graph_view_is_valid`, `graph_view_vertex_count`, `graph_view_is_directed`: O(1)
- `graph_view_node_at`: backing representation lookup cost; O(1) for dense
  graph and heap storage, but potentially O(V) for linked or tree structures
- `graph_view_neighbors`: the backing representation's neighbor-iteration cost
- Space: O(1) for the view itself; structural adapters use no Node or edge map

## Verification

```text
make test NAME=data-structures/graphs/graph-view
make benchmark NAME=data-structures/graphs/graph-view BENCHMARK=graph_view
make benchmark NAME=data-structures/graphs/graph-view BENCHMARK=structural_graph_view
```

At 10,000 operations per sample on this development machine, GraphView wrapper
medians were 12.51 ns/op for vertex count, 17.51 ns/op for index lookup, and
25.96 ns/op for one-neighbor delegation.
