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
typedef bool (*BreadthFirstSearchVisitFn)(Node *node, void *context);

bool breadth_first_search(const GraphView *graph, Node *source,
                          BreadthFirstSearchVisitFn visit, void *context);
```

The checked-in implementation uses the workspace FIFO queue plus dense visited
tracking to traverse GraphView adapters without modifying their graphs.
`GraphView` is the representation-independent graph interface from
`data-structures/graphs/graph-view`. BFS ignores edge weights; adjacency-list,
adjacency-matrix, and imported graph adapters all use the same API. Distance
and parent-tracking variants extend the same shape.

## Contract

- Uses a FIFO queue as the frontier; the queue discipline is what produces
  level order.
- Invokes `visit` in breadth-first discovery order. A `false` visitor result
  stops traversal immediately; caller context owns optional found Node, order,
  count, distance, and parent outputs.
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

## Verification

```text
make test NAME=algorithms/graph-traversal/breadth-first-search
make benchmark NAME=algorithms/graph-traversal/breadth-first-search BENCHMARK=breadth_first_search
```

| Workload | Adjacency list | Adjacency matrix |
| --- | ---: | ---: |
| Full chain traversal | 0.020 ms / 2,000 Nodes | 0.886 ms / 1,000 Nodes |
| Shallow first-enqueued target early exit | 0.0003 ms / 2 visits | 0.0019 ms / 2 visits |

The early-stop workload places the target as the source's first-enqueued
neighbor and a deep distraction branch second. BFS reaches the target after
visiting only source and target; DFS with the same neighbor order would follow
the later-pushed deep branch first. Graph construction is outside the timed
loop.
