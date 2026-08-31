# Kruskal

Minimum-spanning-forest algorithm for an undirected weighted GraphView. It
sorts candidate edges by nondecreasing weight and uses union-find to reject an
edge exactly when it would create a cycle.

## How It Works

Kruskal starts with every graph Node in its own union-find set. It considers
the lightest edge first: if its endpoints belong to different sets, that edge
is accepted and the sets merge; otherwise the edge would close a cycle and is
skipped. The result is a minimum spanning forest: one minimum spanning tree per
connected component.

GraphView supplies the representation-independent graph access. `node_at`
enumerates dense Nodes, neighbor iteration exposes weighted edges, and each
Node's stable `index` becomes its union-find element. An undirected edge appears
twice through GraphView, so Kruskal considers only one canonical direction.

## Required API

```c
typedef struct KruskalEdge {
    Node *from;
    Node *to;
    uint64_t weight;
} KruskalEdge;

bool kruskal_minimum_spanning_forest(
    const GraphView *graph,
    KruskalEdge *out_edges,
    size_t out_edge_capacity,
    size_t *out_edge_count,
    uint64_t *out_total_weight
);
```

## Contract

- Input must be an undirected GraphView with matching weights in both stored
  directions; directed graphs are rejected.
- Each logical input edge is considered once, using the endpoint indexes to
  suppress its mirrored representation.
- Output edges contain no cycle and minimize total weight independently within
  each connected component.
- A disconnected graph returns a minimum spanning forest, not an error.
- `out_edge_capacity` must hold at least every selected output edge. If it is
  insufficient, the function returns `false` without partial output.
- `out_edge_count` reports selected edges and `out_total_weight` reports their
  summed weight on success.
- Empty and single-Node graphs succeed with zero output edges and zero total
  weight.

## Complexity Targets

- Time: O(E log E + E alpha(V)) for edge sorting and union-find
- Space: O(E + V) for collected edges, output, and union-find

## Verification

```text
make test NAME=algorithms/minimum-spanning-trees/kruskal
make benchmark NAME=algorithms/minimum-spanning-trees/kruskal BENCHMARK=kruskal
```

| Graph representation | Forest shape | Median time |
| --- | --- | ---: |
| Adjacency list | 2,000-Node descending-weight chain | 0.089 ms |
| Adjacency matrix | 1,000-Node descending-weight chain | 1.096 ms |

Graph construction is outside the timed loop. Distinct descending weights make
the benchmark include real candidate-edge sorting, union-find cycle detection,
and selected-edge output copying. The matrix workload is slower because every
Node enumeration scans its full matrix row.
