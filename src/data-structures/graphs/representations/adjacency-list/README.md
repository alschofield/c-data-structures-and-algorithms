# Adjacency List

Graph representation storing, for each vertex, a list of its outgoing edges.
The standard choice for sparse graphs.

## Required API

```c
bool adjacency_list_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that. The contract below specifies the graph type and
operations (create/destroy for a fixed vertex count, add_edge, has_edge,
neighbor iteration, vertex/edge counts).

## Contract

- Vertexes are dense indexes `[0, vertex_count)`; out-of-range vertexes are
  rejected cleanly by every operation.
- `add_edge(u, v)` appends v to u's edge list; the directed/undirected policy
  is fixed at creation, and undirected graphs store both directions
  consistently.
- Duplicate edge policy is explicit: either reject duplicates or document
  multigraph behavior; `has_edge` scans only u's list.
- Neighbor iteration visits exactly u's out-edges, each once, in a
  deterministic order.
- Self-loops are permitted unless documented otherwise.
- `destroy` frees all list nodes and the vertex array; the structure owns its
  edges, never caller data.

## Complexity Targets

- `add_edge`: O(1) (amortized, prepend or tail append)
- `has_edge(u, v)`: O(deg(u))
- Iterate neighbors of u: O(deg(u))
- Full traversal of all edges: O(V + E)
- Space: O(V + E)

## Learning Focus

The adjacency list is the representation every graph algorithm in this
curriculum assumes when it claims O(V + E). Implementing it shows why: work is
proportional to edges that actually exist, not to all possible pairs.
Contrasting its O(deg(u)) edge check against the matrix's O(1) makes the
sparse-versus-dense trade-off concrete before BFS, DFS, and Dijkstra build on
top.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
