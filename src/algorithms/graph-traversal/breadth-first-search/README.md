# Breadth-First Search

Level-order graph traversal that explores all vertices at distance k before
any vertex at distance k + 1, using a FIFO queue.

## How It Works

Ripple outward. Starting from the source, visit everything one edge away,
then everything two edges away, ring by ring. The FIFO queue is what creates
that order: vertices enter the frontier in discovery order and leave it in
the same order, so distance-k vertices are fully processed before any
distance-k+1 vertex. That ripple property is why BFS computes minimum-hop
distances on unweighted graphs and why its parent links form a shortest-path
tree. The classic bug: a vertex must be marked visited when it is enqueued,
not when dequeued — otherwise cycles push the same vertex into the queue
repeatedly through different neighbors.

## Required API

```c
typedef struct AdjacencyList AdjacencyList;

bool breadth_first_search(const AdjacencyList *graph, size_t source,
                          size_t *out_order, size_t *out_count);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.
`AdjacencyList` is the graph from
`data-structures/graphs/representations/adjacency-list`. Distance and
parent-tracking variants extend the same shape.

## Contract

- Uses a FIFO queue as the frontier; the queue discipline is what produces
  level order.
- A vertex is marked visited when enqueued, not when dequeued; otherwise the
  same vertex can enter the queue multiple times.
- Visits every vertex reachable from the source exactly once; unreachable
  vertices are never visited.
- On an unweighted graph, the traversal computes minimum-edge-count distances,
  and recorded parent links form a valid shortest-path tree.
- Correct on cyclic graphs, self-loops, and disconnected graphs; an invalid
  source vertex is rejected cleanly.
- The graph is never modified during traversal.

## Complexity Targets

- Time: O(V + E) with an adjacency list
- Space: O(V) for the visited set, queue, and parent array
