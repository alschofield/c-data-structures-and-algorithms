# A* Shortest Path

Searches a nonnegative weighted GraphView from `source` to `goal` with a min-heap ordered by `f = g + heuristic`. With an admissible heuristic, the first accepted goal proposal is optimal.

## C API
```c
typedef uint64_t (*AStarHeuristicFn)(size_t vertex, void *context);
bool a_star(const GraphView *graph, size_t source, size_t goal, AStarHeuristicFn heuristic, void *context, size_t *out_path, size_t out_path_capacity, size_t *out_path_length);
```

## Behavior, Ownership, and Errors
- Requires a valid graph, valid source/goal nodes, non-null heuristic/path/length storage, and nonzero path capacity. A successful path is written source-to-goal and includes both endpoints; `source == goal` writes one vertex.
- The heuristic must be admissible for optimality and must not invalidate graph lifetime/state. Arithmetic overflow in `g + weight` or `g + heuristic`, no route, insufficient output capacity, allocation failure, or GraphView/heap failure returns false. False does not guarantee unchanged output storage.
- The algorithm borrows graph, heuristic context, and outputs; it frees all internal parent, score, heap, and proposal storage.

## Complexity and Verification
With a binary heap, worst-case time is O((V + E) log V) and auxiliary space O(V + E). Verify with `make test NAME=algorithms/shortest-paths/a-star`.
