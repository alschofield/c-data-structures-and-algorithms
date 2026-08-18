# A-Star

Goal-directed shortest-path search that orders the frontier by
`f(n) = g(n) + h(n)`: cost so far plus a heuristic estimate to the goal.

## Required API

```c
typedef struct WeightedGraph WeightedGraph;
typedef uint64_t (*AStarHeuristicFn)(size_t vertex, void *context);

bool a_star(const WeightedGraph *graph, size_t source, size_t goal,
            AStarHeuristicFn heuristic, void *context,
            size_t *out_path, size_t *out_path_length);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.
`WeightedGraph` is the non-negative-weight digraph this module
defines alongside the algorithm.

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

## Complexity Targets

- Time: worst case O((V + E) log V), same as Dijkstra (an uninformative
  heuristic gives no pruning); a strong admissible heuristic prunes most of
  the graph in practice
- Space: O(V) for scores, parents, and the frontier
