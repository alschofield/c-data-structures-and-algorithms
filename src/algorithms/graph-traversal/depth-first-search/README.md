# Depth-First Search

## Implementation Status

Implemented. This document describes the current learner-owned implementation contract.

## How It Works

Traverses vertices reachable from a source using a LIFO frontier. A vertex is marked at push time, avoiding duplicate visits in cyclic graphs.

A stack or recursion follows one branch before backtracking while a visited set prevents repeated work.

## Required API

```c
typedef bool (*DepthFirstSearchVisitFn)(size_t node_index, void *context);
bool depth_first_search(const GraphView *graph, size_t source, DepthFirstSearchVisitFn visit, void *context);
```

## Contract

- Visits source first, then follows LIFO discovery order. Exact ordering among neighbors depends on their GraphView enumeration order and stack pushes.
- Requires a valid graph, non-null visitor, nonempty graph, and valid source index/node. A false visitor, neighbor failure, stack failure, or allocation failure returns false and stops traversal.
- It owns only its temporary discovered flags, index storage, and stack; it borrows the graph and context and releases all temporary storage before returning.

Time is O(V + E) for a normal GraphView; auxiliary space O(V). Verify with `make test NAME=algorithms/graph-traversal/depth-first-search`.

## Complexity Targets

Target: O(V + E) time and O(V) auxiliary space.

## Verification

```sh
make test NAME=<topic-path>
```
