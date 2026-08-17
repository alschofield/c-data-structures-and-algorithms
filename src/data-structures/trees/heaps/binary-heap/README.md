# Binary Heap

Array-backed complete binary tree maintaining the heap property: every parent
orders before its children under the caller's comparison.

## Required API

```c
bool binary_heap_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that. The contract below specifies the heap type and
operations (create/destroy with a comparison function, push, pop, peek,
size/is_empty).

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

## Learning Focus

The binary heap is the standard priority queue and the engine inside heap
sort, Dijkstra, and A-star. Implementing it teaches the implicit-tree trick —
a complete tree needs no pointers — and the two dual restoration operations,
sift-up and sift-down. Understanding why bottom-up construction is O(n) while
n pushes are O(n log n) is a classic amortized-analysis exercise.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
