# Graph View

Representation-independent read-only graph interface. A GraphView adapts an
adjacency list, adjacency matrix, imported graph, or future graph structure so
algorithms can enumerate weighted outgoing edges without knowing the backing
representation.

## How It Works

Graph algorithms do not need a graph's fields; they need a node count, dense
Node lookup, and a way to visit one node's outgoing neighbors and weights.
GraphView packages those operations as a context pointer plus function
pointers. `Node` and
`Edge` define shared graph identity and weighted connections; representations
may leave unused fields empty when their storage lives elsewhere. Each concrete
representation creates a view whose callbacks read that representation. An
external graph can create its own view adapter without changing its source
code. Unweighted graphs expose every edge with weight `1U`, so the same view
works for BFS/DFS and for weighted shortest paths.

## Required API

```c
typedef struct Node Node;
typedef struct Edge Edge;

struct Edge {
    Node *target;
    uint64_t weight;
};

struct Node {
    void *value;
    size_t index;
    void *owner;
    size_t edge_count;
    size_t edge_capacity;
    Edge **edges;
};

typedef struct GraphView GraphView;
typedef bool (*GraphViewVisitFn)(Node *neighbor, uint64_t weight,
                                 void *context);
typedef size_t (*GraphViewVertexCountFn)(const void *graph_context);
typedef bool (*GraphViewNodeAtFn)(const void *graph_context, size_t index,
                                  Node **out_node);
typedef bool (*GraphViewNeighborsFn)(const void *graph_context,
                                      const Node *node,
                                      GraphViewVisitFn visit,
                                      void *context);

struct GraphView {
    const void *context;
    GraphViewVertexCountFn vertex_count;
    GraphViewNodeAtFn node_at;
    GraphViewNeighborsFn neighbors;
};

bool graph_view_is_valid(const GraphView *view);
size_t graph_view_vertex_count(const GraphView *view);
bool graph_view_node_at(const GraphView *view, size_t index, Node **out_node);
bool graph_view_neighbors(const GraphView *view, const Node *node,
                           GraphViewVisitFn visit, void *context);
```

## Contract

- A valid view has non-`NULL` context, `vertex_count`, `node_at`, and
  `neighbors` fields.
- Every Node has a stable dense `index` in its owning graph.
- `graph_view_node_at` returns a graph-owned Node for a valid dense index.
- `graph_view_neighbors` visits each outgoing edge exactly once, passing its
  target Node and nonnegative `uint64_t` weight.
- A visitor returning `false` stops iteration and makes
  `graph_view_neighbors` return `false`.
- The view never owns, copies, mutates, or destroys its backing graph.
- Concrete graph representations expose a function that fills an output
  `GraphView`; imported graphs may construct the struct directly with adapter
  callbacks.

## Complexity Targets

- `graph_view_is_valid`, `graph_view_vertex_count`, `graph_view_node_at`: O(1)
- `graph_view_neighbors`: the backing representation's neighbor-iteration cost
- Space: O(1) for the view itself

## Verification

```text
make test NAME=data-structures/graphs/graph-view
make benchmark NAME=data-structures/graphs/graph-view BENCHMARK=graph_view
```

At 10,000 operations per sample on this development machine, GraphView wrapper
medians were 3.28 ns/op for node count, 3.15 ns/op for dense Node lookup, and
3.07 ns/op for one-neighbor delegation.
