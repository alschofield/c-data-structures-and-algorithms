# Graph View

Representation-independent read-only graph interface. A GraphView adapts an
adjacency list, adjacency matrix, imported graph, or future graph structure so
algorithms can enumerate weighted outgoing edges without knowing the backing
representation.

## How It Works

Graph algorithms do not need a graph's fields; they need only a vertex count
and a way to visit one vertex's outgoing neighbors and weights. GraphView
packages those operations as a context pointer plus function pointers. Each
concrete representation creates a view whose callbacks read that
representation. An external graph can create its own view adapter without
changing its source code. Unweighted graphs expose every edge with weight
`1U`, so the same view works for BFS/DFS and for weighted shortest paths.

## Required API

```c
typedef struct GraphView GraphView;
typedef bool (*GraphViewVisitFn)(size_t neighbor, uint64_t weight,
                                 void *context);
typedef size_t (*GraphViewVertexCountFn)(const void *graph_context);
typedef bool (*GraphViewNeighborsFn)(const void *graph_context, size_t vertex,
                                     GraphViewVisitFn visit, void *context);

struct GraphView {
    const void *context;
    GraphViewVertexCountFn vertex_count;
    GraphViewNeighborsFn neighbors;
};

bool graph_view_is_valid(const GraphView *view);
size_t graph_view_vertex_count(const GraphView *view);
bool graph_view_neighbors(const GraphView *view, size_t vertex,
                          GraphViewVisitFn visit, void *context);
```

## Contract

- A valid view has non-`NULL` context, `vertex_count`, and `neighbors` fields.
- Vertexes are dense indexes `[0, vertex_count)`.
- `graph_view_neighbors` visits each outgoing edge exactly once, passing its
  target vertex and nonnegative `uint64_t` weight.
- A visitor returning `false` stops iteration and makes
  `graph_view_neighbors` return `false`.
- The view never owns, copies, mutates, or destroys its backing graph.
- Concrete graph representations expose a function that fills an output
  `GraphView`; imported graphs may construct the struct directly with adapter
  callbacks.

## Complexity Targets

- `graph_view_is_valid`, `graph_view_vertex_count`: O(1)
- `graph_view_neighbors`: the backing representation's neighbor-iteration cost
- Space: O(1) for the view itself
