# Depth-First Search

Graph traversal that explores as far as possible along each branch before
backtracking, using recursion or an explicit stack.

## Required API

```c
typedef struct AdjacencyList AdjacencyList;

bool depth_first_search(const AdjacencyList *graph, size_t source,
                        size_t *out_order, size_t *out_count);
```

The checked-in source is still the scaffold gate `bool
depth_first_search_is_implemented(void)`, which returns `false`; the test asserts exactly that.
`AdjacencyList` is the graph from
`data-structures/graphs/representations/adjacency-list`.
Discovery/finish-time variants extend the same shape.

## Contract

- Visits every vertex reachable from the source exactly once; a visited set
  is mandatory to terminate on cyclic graphs.
- The recursive and explicit-stack forms must both be understood; recursion
  depth is O(V) in the worst case, so deep graphs favor the explicit stack.
- Discovery/finish ordering must be consistent with DFS semantics: a vertex
  finishes only after all vertices reachable through its unvisited neighbors
  finish. This ordering is what topological sort and cycle detection build on.
- Correct on cyclic graphs, self-loops, and disconnected graphs; an invalid
  source vertex is rejected cleanly.
- The graph is never modified during traversal.

## Complexity Targets

- Time: O(V + E) with an adjacency list
- Space: O(V) for the visited set plus recursion/stack depth up to O(V)
