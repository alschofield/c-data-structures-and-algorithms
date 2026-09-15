# Binary Heap

An opaque array-backed min heap of borrowed pointers. The comparator defines priority: a lower result has higher priority.

## C API
```c
typedef int (*BinaryHeapCompareFn)(const void *left, const void *right); BinaryHeap *binary_heap_create(BinaryHeapCompareFn compare); void binary_heap_destroy(BinaryHeap *heap);
bool binary_heap_push(BinaryHeap *heap, void *item); bool binary_heap_pop(BinaryHeap *heap, void **out_item); bool binary_heap_peek(const BinaryHeap *heap, void **out_item);
size_t binary_heap_size(const BinaryHeap *heap); bool binary_heap_is_empty(const BinaryHeap *heap); bool binary_heap_graph_view(const BinaryHeap *heap, GraphView *out_view);
```

## Behavior, Ownership, and Invariants
- Create requires a comparator. Pop/peek require a nonempty heap and output storage. Parent priority never follows a lower-priority child.
- Growth reserves full levels (3, 7, 15, ...) and rejects arithmetic/allocation failure without changing heap fields. Payloads are borrowed and may be null only when the comparator supports them; destroy frees no payload.
- The direct, borrowed GraphView exposes heap array indexes with directed unit edges from parent to children and expires on mutation/destruction.

## Complexity and Verification
Push/pop are O(log n); peek, size, and empty are O(1). Verify with `make test NAME=data-structures/trees/heaps/binary-heap`.
