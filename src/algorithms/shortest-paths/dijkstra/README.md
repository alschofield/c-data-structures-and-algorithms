# Dijkstra Shortest Paths

Computes shortest paths from one source using a min-heap of distance proposals. Stale proposals are discarded and settled vertices are expanded once.

## C API
```c
#define DIJKSTRA_INFINITY UINT64_MAX
bool dijkstra(const GraphView *graph, size_t source, uint64_t *out_distances, size_t *out_parents);
```

## Behavior, Outputs, and Errors
- Requires a valid nonempty GraphView, valid source node, and caller arrays sized for every graph vertex. On success, unreachable distances are `DIJKSTRA_INFINITY` with parent `SIZE_MAX`; source distance is zero and parent is itself.
- Edge weights are `uint64_t` and must be nonnegative by type. A relaxation whose addition would overflow fails. Graph/heap/allocation failures also return false; arrays are initialized before later internal failures, so false does not promise unchanged outputs.
- The function borrows graph and output arrays, while owning/freeing its temporary settled array and heap proposals.

## Complexity and Verification
Time is O((V + E) log V) with duplicate heap proposals; auxiliary space O(V + E). Verify with `make test NAME=algorithms/shortest-paths/dijkstra`.
