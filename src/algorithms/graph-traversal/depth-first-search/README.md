# Depth-First Search

Graph traversal that explores as far as possible along each branch before
backtracking, using recursion or an explicit stack.

## Required API

```c
bool depth_first_search_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

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
- Scaffold gate: `depth_first_search_is_implemented` returns `false`; the test
  asserts exactly that.

## Complexity Targets

- Time: O(V + E) with an adjacency list
- Space: O(V) for the visited set plus recursion/stack depth up to O(V)

## Learning Focus

DFS is the substrate for a large family of graph algorithms: topological sort,
cycle detection, connected and strongly connected components, and edge
classification all fall out of its discovery/finish structure. Implementing
both the recursive and iterative forms teaches how the call stack is an
implicit data structure and what changes when you make it explicit.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
