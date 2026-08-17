# Dijkstra

Single-source shortest paths on a weighted graph with non-negative edge
weights, driven by a min-priority queue over tentative distances.

## Required API

```c
bool dijkstra_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

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
- Scaffold gate: `dijkstra_is_implemented` returns `false`; the test asserts
  exactly that.

## Complexity Targets

- Time: O((V + E) log V) with a binary heap
- Space: O(V) for distances, parents, and the heap

## Learning Focus

Dijkstra is BFS generalized to weighted graphs: the FIFO queue becomes a
priority queue and levels become tentative distances. Implementing it teaches
the greedy exchange argument for why settled vertices are final, why that
argument collapses under negative weights, and the practical lazy-deletion
technique for priority queues that lack decrease-key.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
