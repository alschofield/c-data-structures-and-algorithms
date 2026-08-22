# Data Structures and Algorithms in C

The source tree is the curriculum. Completed and scaffold topics contain a
contract README, public header, implementation source, and dedicated assertion
test. Follow-on contract-only topics define the next learning target before
their source files are created.

## Taxonomy

```text
src/data-structures/linear/arrays/dynamic-array                 complete
src/data-structures/linear/stacks/stack                         complete
src/data-structures/linear/queues/queue                         complete
src/data-structures/linear/linked/singly-linked-list           complete
src/data-structures/linear/linked/doubly-linked-list           complete
src/data-structures/associative/hash-tables/separate-chaining   complete
src/data-structures/associative/hash-tables/resizable-separate-chaining
src/data-structures/trees/binary-search-trees/binary-search-tree complete
src/data-structures/trees/tries/prefix-trie                      complete
src/data-structures/trees/heaps/binary-heap
src/data-structures/graphs/{representations/{adjacency-list,adjacency-matrix},disjoint-sets/union-find}
src/algorithms/searching/{linear-search,binary-search}
src/algorithms/sorting/comparison/bubble-sort                  complete
src/algorithms/sorting/comparison/selection-sort               complete
src/algorithms/sorting/comparison/insertion-sort               complete
src/algorithms/sorting/comparison/merge-sort                   complete
src/algorithms/sorting/comparison/{quick,heap}-sort
src/algorithms/sorting/non-comparison/{counting,radix}-sort
src/algorithms/graph-traversal/{breadth-first-search,depth-first-search}
src/algorithms/shortest-paths/{dijkstra,a-star}
```

Entries not labelled complete are empty learning scaffolds: their README
defines the target API and behavior, while the header and source are left for
you to write. Their full test suite becomes buildable once the declarations
exist and passes once the implementation is correct.

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

Stack, queue, dynamic array, singly and doubly linked lists, separate-chaining
hash table, binary search tree, prefix trie, and bubble, selection, insertion,
and merge sort have benchmark sources. The dependency-free harness reports
repeated-sample timing; it is not a complexity proof. See
[bench/README.md](bench/README.md) for full results, scaling comparisons, and
analysis of why each structure performs the way it does.

Implement learning modules from first principles. Do not substitute library
containers, search routines, or sorting routines for the exercise.
