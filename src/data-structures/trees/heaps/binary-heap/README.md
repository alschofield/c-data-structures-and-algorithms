# Binary Heap

Array-backed complete binary tree maintaining the heap property: every parent
orders before its children under the caller's comparison.

## Required API

```c
typedef struct BinaryHeap BinaryHeap;
typedef int (*BinaryHeapCompareFn)(const void *left, const void *right);

BinaryHeap *binary_heap_create(BinaryHeapCompareFn compare);
void binary_heap_destroy(BinaryHeap *heap);
bool binary_heap_push(BinaryHeap *heap, void *item);
bool binary_heap_pop(BinaryHeap *heap, void **out_item);
bool binary_heap_peek(const BinaryHeap *heap, void **out_item);
size_t binary_heap_size(const BinaryHeap *heap);
bool binary_heap_is_empty(const BinaryHeap *heap);
```

The checked-in source is a failing stub; the tests define the expected
behavior and pass only once the implementation is written.

## Contract

- Implicit array layout: children of index `i` live at `2i + 1` and `2i + 2`,
  parent at `(i - 1) / 2`; no node allocation.
- The tree is always complete: elements occupy indexes `[0, size)` with no
  gaps, which is what makes the array encoding valid.
- `push` appends then sifts up; `pop` swaps the root with the last element,
  shrinks, then sifts down. After either, the heap property holds everywhere.
- `pop` and `peek` return the extreme element; `peek` does not remove. Both
  fail cleanly on an empty heap without touching output parameters.
- Equal-priority elements dequeue in no guaranteed order; the heap is not
  stable and callers must not rely on insertion order among equals.
- Backing storage grows geometrically; allocation failure leaves the heap
  unchanged. `destroy` frees heap-owned storage only, never stored values.

## Complexity Targets

- `push`: O(log n) (amortized, including geometric growth)
- `pop`: O(log n)
- `peek`, `size`, `is_empty`: O(1)
- Build from n items via bottom-up heapify: O(n)
- Space: O(n) contiguous, no per-element pointer overhead
