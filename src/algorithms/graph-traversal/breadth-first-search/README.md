# Breadth-First Search

Traverses vertices reachable from a source using a FIFO frontier. A vertex is marked when enqueued, so cycles and converging paths cannot visit it twice.

## C API
```c
typedef bool (*BreadthFirstSearchVisitFn)(size_t node_index, void *context);
bool breadth_first_search(const GraphView *graph, size_t source, BreadthFirstSearchVisitFn visit, void *context);
```

## Behavior, Allocation, and Errors
- Visits source first, then reachable vertices in breadth-first discovery order. Exact sibling order follows the backing `GraphView` neighbor order.
- Requires a valid graph, non-null visitor, nonempty graph, and a valid source index/node. A visitor returning false, a graph neighbor error, queue failure, or allocation failure returns false and stops traversal.
- It allocates internal discovered flags, stable index storage, and a queue, then frees all of them before return. It never owns the graph or visitor context.

## Complexity and Verification
Time is O(V + E) for a normal GraphView; auxiliary space O(V). Verify with `make test NAME=algorithms/graph-traversal/breadth-first-search`.
