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
src/data-structures/trees/binary-search-trees/binary-search-tree complete
src/data-structures/trees/tries/prefix-trie                      complete
src/data-structures/trees/heaps/binary-heap
src/data-structures/graphs/{graph-view,representations/{adjacency-list,adjacency-matrix},disjoint-sets/union-find}
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

## Choosing a Structure or Algorithm

Start with the access pattern, not the name of a structure. Ask: do I know an
index, need order, need a prefix, need the next highest-priority item, or need
to traverse relationships? The table is a first decision guide; each topic
README defines its exact contract and tradeoffs.

### Data Structures

| Need | Choose | Key property | Avoid when |
| --- | --- | --- | --- |
| Indexed contiguous storage, append-heavy work | Dynamic array | O(1) indexing, amortized O(1) append | Frequent middle insert/remove shifts are expensive |
| Last item added must leave first | Stack | O(1) push/pop at one end | You need FIFO order or arbitrary lookup |
| First item added must leave first | Queue | O(1) amortized enqueue, O(1) dequeue | You need priority order or arbitrary lookup |
| Cheap head insertion/removal | Singly linked list | O(1) at head; no shifting | You need indexing or efficient tail work |
| Efficient work at both ends | Doubly linked list | O(1) push/pop at both ends | Memory locality or indexed access matters most |
| Exact lookup by key | Resizable hash table | O(1) expected lookup with bounded load factor | You need sorted/range/prefix queries |
| Learn collision handling with fixed buckets | Fixed separate-chaining hash table | Simple collision chains | Production-scale lookup; chains grow with entries |
| Sorted keys, ranges, in-order traversal | Binary search tree | O(log n) average ordered lookup | Input can be sorted/adversarial without balancing |
| Repeatedly take min/max priority | Binary heap | O(log n) push/pop, O(1) peek | You need arbitrary search or sorted iteration |
| Key/prefix lookup or autocomplete | Prefix trie | O(m) by key length; natural prefix paths | You only need exact lookup and memory is tight |
| Sparse graph and neighbor traversal | Adjacency list | O(V + E) traversal, O(V + E) space | Constant-time edge tests dominate |
| Dense graph and constant-time edge tests | Adjacency matrix | O(1) edge test | Graph is sparse or neighbor iteration dominates |
| Repeated group merging/connectivity checks | Union-find | Near-constant union/find | You need paths, order, or graph traversal |

### Algorithms

| Need | Choose | Key requirement | Avoid when |
| --- | --- | --- | --- |
| Find a value in unsorted data | Linear search | None | Input is sorted or repeatedly searched |
| Find a value in sorted data | Binary search | Input remains sorted | Data is unsorted or insertions are frequent |
| Teach adjacent swapping; nearly sorted tiny input | Bubble sort | Early exit helps sorted input | General-purpose sorting |
| Minimize writes/swaps | Selection sort | At most n - 1 swaps | Comparison count or sorted-input speed matters |
| Sort tiny or nearly sorted input | Insertion sort | Cost tracks displacement; stable | Large randomly ordered input |
| Stable, predictable O(n log n) sorting | Merge sort | O(n) auxiliary buffer | Memory is constrained |
| Fast general in-place comparison sort | Quick sort | Pivot strategy must defend input shape | Guaranteed worst-case time or stability is required |
| Guaranteed O(n log n), in-place sort | Heap sort | Heapify then repeated extract-max | Stability is required |
| Sort small-range integer keys | Counting sort | Known bounded key range | Key range is huge relative to item count |
| Sort fixed-width integer digits | Radix sort | Stable per-digit sort | Keys are not digit-addressable integers/bytes |
| Fewest-hop paths in unweighted graph | Breadth-first search | FIFO frontier, visited on enqueue | Edge weights matter |
| Explore graph depth, cycle/topology groundwork | Depth-first search | Stack/recursion plus visited set | You need fewest-hop paths |
| Cheapest paths with nonnegative weights | Dijkstra | Min-priority queue; no negative edges | Negative weights or a strongly goal-directed search |
| Cheapest path toward a known goal | A* | Admissible heuristic | No useful heuristic exists; use Dijkstra instead |

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
