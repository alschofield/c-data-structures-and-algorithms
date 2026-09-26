# Kruskal Minimum Spanning Forest

## Implementation Status

Target scaffold. Production behavior is not claimed until the learner-owned implementation and its verification are complete.

## How It Works

Collects canonical undirected edges, orders them by weight and endpoints, and uses union-find to select every edge that joins two components. Disconnected graphs produce a minimum spanning forest.

This leaf defines the required data-structure or algorithm behavior and its observable boundary conditions.

## Required API

```c
typedef struct KruskalEdge { size_t from; size_t to; uint64_t weight; } KruskalEdge;
bool kruskal_minimum_spanning_forest(const GraphView *graph, KruskalEdge *out_edges, size_t out_edge_capacity, size_t *out_edge_count, uint64_t *out_total_weight);
```

## Contract

- Requires a valid, undirected GraphView and non-null count/weight outputs. Empty and single-vertex graphs succeed with count and total weight zero without requiring `out_edges`; larger graphs require it.
- Each selected edge has canonical `from < to`. `out_edges` receives the selected forest only after capacity is checked; its capacity must cover at most `V - component_count` edges. Equal weights are deterministically ordered by endpoints.
- The algorithm owns/freees candidate, sort-pointer, and union-find storage; it borrows graph and outputs. It fails on allocation/size/weight-total overflow, invalid node/neighbor iteration, directed input, or insufficient output capacity. Failed calls need not preserve output scalars.

Time is O(E log E + E alpha(V)); auxiliary space O(V + E). Verify with `make test NAME=algorithms/minimum-spanning-trees/kruskal`.

## Complexity Targets

Target complexity is the topic-specific bound stated in the existing contract; add a measured benchmark only after an implementation exists.

## Verification

```sh
make test NAME=<topic-path>
```
