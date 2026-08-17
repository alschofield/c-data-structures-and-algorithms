# Breadth-First Search

Level-order graph traversal that explores all vertices at distance k before
any vertex at distance k + 1, using a FIFO queue.

## Required API

```c
bool breadth_first_search_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

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
- Scaffold gate: `breadth_first_search_is_implemented` returns `false`; the
  test asserts exactly that.

## Complexity Targets

- Time: O(V + E) with an adjacency list
- Space: O(V) for the visited set, queue, and parent array

## Learning Focus

BFS shows that a traversal's order is entirely a property of its frontier data
structure — swap the queue for a stack and you get DFS from the same skeleton.
The enqueue-time-marking rule is a classic correctness subtlety worth getting
wrong once and understanding. BFS is also the foundation Dijkstra generalizes:
same pattern, priority queue instead of FIFO.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
