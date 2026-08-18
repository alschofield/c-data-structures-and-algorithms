# Data Structures and Algorithms in C

The source tree is the curriculum. Every leaf topic contains a contract README, public header, implementation source, and dedicated assertion test.

## Taxonomy

```text
src/data-structures/linear/arrays/dynamic-array                 complete
src/data-structures/linear/stacks/stack                         complete
src/data-structures/linear/queues/queue                         complete
src/data-structures/linear/linked/singly-linked-list           complete
src/data-structures/linear/linked/doubly-linked-list
src/data-structures/associative/hash-tables/separate-chaining   complete
src/data-structures/trees/binary-search-trees/binary-search-tree complete
src/data-structures/trees/{heaps/binary-heap,tries/prefix-trie}
src/data-structures/graphs/{representations/{adjacency-list,adjacency-matrix},disjoint-sets/union-find}
src/algorithms/searching/{linear-search,binary-search}
src/algorithms/sorting/{comparison/{bubble,selection,insertion,merge,quick,heap}-sort,non-comparison/{counting,radix}-sort}
src/algorithms/graph-traversal/{breadth-first-search,depth-first-search}
src/algorithms/shortest-paths/{dijkstra,a-star}
```

Entries not labelled complete are failing-stub scaffolds: the header declares
the target API, the source stubs it out, and the full test suite defines the
expected behavior. `make test` fails until the topic is implemented.

## Commands

```bash
make test NAME=data-structures/linear/stacks/stack
make test NAME=data-structures/associative/hash-tables/separate-chaining
make test NAME=algorithms/searching/linear-search
make benchmark NAME=data-structures/linear/stacks/stack BENCHMARK=stack
make benchmark NAME=data-structures/linear/arrays/dynamic-array BENCHMARK=dynamic_array
make benchmark NAME=data-structures/trees/binary-search-trees/binary-search-tree BENCHMARK=binary_search_tree
make benchmark-test
```

Only stack, queue, singly linked list, dynamic array, separate-chaining hash table, and binary search tree have benchmark sources. The dependency-free harness reports repeated-sample timing; it is not a complexity proof. See [bench/README.md](bench/README.md) for full results, scaling comparisons, and analysis of why each structure performs the way it does.

Implement learning modules from first principles. Do not substitute library search or sorting routines for the exercise.
