# Dijkstra Shortest Paths

## Implementation Status

Implemented. This document describes the current learner-owned implementation contract.

## How It Works

Computes shortest paths from one source using a min-heap of distance proposals. Stale proposals are discarded and settled vertices are expanded once.

A min-priority frontier settles the smallest tentative non-negative path cost and relaxes outgoing edges.

## Required API

```c
#define DIJKSTRA_INFINITY UINT64_MAX
bool dijkstra(const GraphView *graph, size_t source, uint64_t *out_distances, size_t *out_parents);
```

## Contract

- Requires a valid nonempty GraphView, valid source node, and caller arrays sized for every graph vertex. On success, unreachable distances are `DIJKSTRA_INFINITY` with parent `SIZE_MAX`; source distance is zero and parent is itself.
- Edge weights are `uint64_t` and must be nonnegative by type. A relaxation whose addition would overflow fails. Graph/heap/allocation failures also return false; arrays are initialized before later internal failures, so false does not promise unchanged outputs.
- The function borrows graph and output arrays, while owning/freeing its temporary settled array and heap proposals.

Time is O((V + E) log V) with duplicate heap proposals; auxiliary space O(V + E). Verify with `make test NAME=algorithms/shortest-paths/dijkstra`.

## Complexity Targets

Target: O((V + E) log V) time and O(V) auxiliary space.

## Verification

```sh
make test NAME=<topic-path>
```
