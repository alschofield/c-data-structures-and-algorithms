# A-Star

Goal-directed shortest-path search that orders the frontier by
`f(n) = g(n) + h(n)`: cost so far plus a heuristic estimate to the goal.

## Required API

```c
bool a_star_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

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
- Scaffold gate: `a_star_is_implemented` returns `false`; the test asserts
  exactly that.

## Complexity Targets

- Time: worst case O((V + E) log V), same as Dijkstra (an uninformative
  heuristic gives no pruning); a strong admissible heuristic prunes most of
  the graph in practice
- Space: O(V) for scores, parents, and the frontier

## Learning Focus

A-star shows how domain knowledge slots into a general algorithm without
breaking its guarantees: the heuristic reshapes exploration order while
admissibility preserves optimality. Implementing it clarifies the
admissible/consistent distinction and why the goal test must happen at
extraction, not at edge relaxation — testing early sacrifices optimality.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
