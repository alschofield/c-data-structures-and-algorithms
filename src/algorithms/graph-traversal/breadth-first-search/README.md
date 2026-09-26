# Breadth-First Search

## Implementation Status

Implemented. This document describes the current learner-owned implementation contract.

## How It Works

Traverses vertices reachable from a source using a FIFO frontier. A vertex is marked when enqueued, so cycles and converging paths cannot visit it twice.

A FIFO frontier visits each discovered vertex layer by layer while a visited set prevents repeated work.

## Required API

```c
typedef bool (*BreadthFirstSearchVisitFn)(size_t node_index, void *context);
bool breadth_first_search(const GraphView *graph, size_t source, BreadthFirstSearchVisitFn visit, void *context);
```

## Contract

- Visits source first, then reachable vertices in breadth-first discovery order. Exact sibling order follows the backing `GraphView` neighbor order.
- Requires a valid graph, non-null visitor, nonempty graph, and a valid source index/node. A visitor returning false, a graph neighbor error, queue failure, or allocation failure returns false and stops traversal.
- It allocates internal discovered flags, stable index storage, and a queue, then frees all of them before return. It never owns the graph or visitor context.

Time is O(V + E) for a normal GraphView; auxiliary space O(V). Verify with `make test NAME=algorithms/graph-traversal/breadth-first-search`.

## Complexity Targets

Target: O(V + E) time and O(V) auxiliary space.

## Verification

```sh
make test NAME=<topic-path>
```
