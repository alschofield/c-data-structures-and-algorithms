# A-Star

Goal-directed shortest-path search that orders the frontier by
`f(n) = g(n) + h(n)`: cost so far plus a heuristic estimate to the goal.

## How It Works

Dijkstra with a sense of direction. Plain Dijkstra explores a blind circle
around the source; A* orders its frontier by f(n) = g(n) + h(n) — cost paid
so far plus a heuristic guess of the cost remaining — stretching the search
ellipse toward the goal and skipping regions that cannot help. The one law is
admissibility: the heuristic must never overestimate the true remaining cost.
Underestimating merely explores more; overestimating can commit the search to
a wrong path and return a non-optimal answer. With h(n) = 0 everywhere the
guess contributes nothing and A* degenerates into exactly Dijkstra — which
the tests check directly.

## Required API

```c
typedef struct GraphView GraphView;
typedef uint64_t (*AStarHeuristicFn)(size_t vertex, void *context);

bool a_star(const GraphView *graph, Node *source, Node *goal,
            AStarHeuristicFn heuristic, void *context,
            size_t *out_path, size_t out_path_capacity,
            size_t *out_path_length);
```

`GraphView` is the representation-independent non-negative weighted graph
interface from `data-structures/graphs/graph-view`.

`source` and `goal` are graph-owned Nodes, matching Dijkstra's source
boundary. `out_path` contains their dense Node indexes in source-to-goal
order, and `out_path_length` receives the number of written indexes.
`out_path_capacity` is the number of writable `size_t` entries in `out_path`.

## Contract

- The heuristic must be admissible (never overestimates the true remaining
  cost) for the returned path to be optimal; a consistent (monotone)
  heuristic additionally guarantees no vertex needs re-expansion.
- With `h(n) = 0` for all n, the algorithm must degenerate to exactly
  Dijkstra's behavior.
- Frontier is a min-priority queue keyed on `f`; ties are resolved
  deterministically.
- Precondition: non-negative edge weights, as with Dijkstra.
- Terminates with the optimal path when the goal is extracted from the
  frontier (given an admissible heuristic), or reports no-path when the
  frontier empties.
- Parent links must reconstruct the returned path; unreachable goals are an
  explicit no-path result.
- Returns `false` without writing output when `out_path_capacity` cannot hold
  the complete route. A simple path can contain at most the graph's vertex
  count entries.

## Complexity Targets

- Time: worst case O((V + E) log V), same as Dijkstra (an uninformative
  heuristic gives no pruning); a strong admissible heuristic prunes most of
  the graph in practice
- Space: O(V) for scores, parents, and the frontier

## Verification

```text
make test NAME=algorithms/shortest-paths/a-star
make benchmark NAME=algorithms/shortest-paths/a-star BENCHMARK=a_star
```

| Graph representation | Path shape | Median time |
| --- | --- | ---: |
| Adjacency list | 2,000-Node unit-weight chain, `h(n) = 0` | 0.093 ms |
| Adjacency matrix | 1,000-Node unit-weight chain, `h(n) = 0` | 1.236 ms |

Graph construction is outside the timed loop. The zero heuristic establishes
the Dijkstra-equivalent baseline; each run includes heap proposals, stale-entry
checks, source-to-goal parent reconstruction, and output-capacity validation.
The matrix workload is slower because each expanded Node scans its full row.
