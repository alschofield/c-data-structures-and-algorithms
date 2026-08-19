# Dijkstra

Single-source shortest paths on a weighted graph with non-negative edge
weights, driven by a min-priority queue over tentative distances.

## How It Works

BFS grown up to handle weighted edges: the ripple expands by total path cost
instead of hop count. Every vertex carries a tentative best-known distance
(infinity at the start). Repeatedly extract the cheapest unsettled vertex —
this is why the min-priority queue exists — and settle it: no cheaper route
to it can exist, because any alternative would have to pass through something
already more expensive. That settlement argument is exactly what negative
edge weights break, which is why they must be rejected.

Settling a vertex relaxes its edges: for each neighbor, if going through the
settled vertex beats the neighbor's current best, update the distance and
record the settled vertex as its parent. The parent links reconstruct the
actual shortest path once the goal settles.

## Required API

```c
#define DIJKSTRA_INFINITY UINT64_MAX

typedef struct WeightedGraph WeightedGraph;

bool dijkstra(const WeightedGraph *graph, size_t source,
              uint64_t *out_distances, size_t *out_parents);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.
`WeightedGraph` is the non-negative-weight digraph this module
defines alongside the algorithm. Unreachable vertexes report
`DIJKSTRA_INFINITY`.

## Contract

- Precondition: all edge weights are non-negative. Negative weights break the
  greedy settlement argument and must be rejected, not silently mis-answered.
- A vertex's distance is final when it is extracted from the priority queue;
  it is never revisited afterward.
- Relaxation: for edge (u, v, w), if `dist[u] + w < dist[v]`, update `dist[v]`
  and `parent[v]`. With a binary heap, either decrease-key or lazy insertion
  with stale-entry skipping is acceptable; stale entries must be detectably
  skipped.
- Unreachable vertices report an explicit infinite distance, never a garbage
  value.
- Parent links must reconstruct an actual shortest path from the source.
- Correct on graphs with cycles, parallel edges, and self-loops; an invalid
  source vertex is rejected cleanly.

## Complexity Targets

- Time: O((V + E) log V) with a binary heap
- Space: O(V) for distances, parents, and the heap
