# Depth-First Search

Graph traversal that explores as far as possible along each branch before
backtracking, using recursion or an explicit stack.

## How It Works

Charge down one path as far as it goes; back up only at dead ends and try
the next branch — maze-running with a hand on the wall. The stack (explicit,
or the call stack via recursion) creates that order, and the visited set is
what terminates loops on cyclic graphs. Swap DFS's stack for a queue and it
becomes BFS; the frontier discipline is the entire difference between them.
DFS's finish ordering — a vertex finishes only after everything reachable
through it finishes — is the foundation topological sort and cycle detection
build on.

## Required API

```c
typedef bool (*DepthFirstSearchVisitFn)(Node *node, void *context);

bool depth_first_search(const GraphView *graph, Node *source,
                        DepthFirstSearchVisitFn visit, void *context);
```

The checked-in implementation uses the workspace LIFO stack plus dense visited
tracking to traverse GraphView adapters without modifying their graphs.
`GraphView` is the representation-independent graph interface from
`data-structures/graphs/graph-view`. DFS ignores edge weights; adjacency-list,
adjacency-matrix, and imported graph adapters all use the same API.
Discovery/finish-time variants extend the same shape.

## Contract

- Visits every vertex reachable from the source exactly once; a visited set
  is mandatory to terminate on cyclic graphs.
- Invokes `visit` in depth-first discovery order. A `false` visitor result
  stops traversal immediately; caller context owns optional found Node, order,
  count, discovery, and finish outputs.
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

## Verification

```text
make test NAME=algorithms/graph-traversal/depth-first-search
make benchmark NAME=algorithms/graph-traversal/depth-first-search BENCHMARK=depth_first_search
```

| Workload | Adjacency list | Adjacency matrix |
| --- | ---: | ---: |
| Full chain traversal | 0.015 ms / 2,000 Nodes | 0.834 ms / 1,000 Nodes |
| Deep last-enumerated target early exit | 0.0005 ms / 3 visits | 0.0025 ms / 3 visits |

The early-stop workload places a wide distraction branch first and the target
branch last in neighbor enumeration. The LIFO stack pops the last-enumerated
deep branch first, reaching its target after source, branch root, and target.
Graph construction is outside the timed loop.
