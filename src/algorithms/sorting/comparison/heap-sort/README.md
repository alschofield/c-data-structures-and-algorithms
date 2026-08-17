# Heap Sort

Comparison sort that builds a max-heap in the array, then repeatedly swaps the
root to the tail and re-heapifies the shrinking prefix.

## Required API

```c
bool heap_sort_is_implemented(void);
```

The header exposes only this scaffold gate. The source returns `false` and the
test asserts exactly that.

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Not stable; sift operations reorder equal elements. Do not claim stability.
- Heap construction must use bottom-up heapify (sift-down from the last
  parent), which is O(n), not n repeated insertions.
- Uses implicit array indexing: children of `i` at `2i + 1` and `2i + 2`;
  no node allocation.
- Guaranteed O(n log n) regardless of input order.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- Scaffold gate: `heap_sort_is_implemented` returns `false`; the test asserts
  exactly that.

## Complexity Targets

- Best: O(n log n)
- Average: O(n log n)
- Worst: O(n log n)
- Space: O(1), in place and iterative

## Learning Focus

Heap sort is the only classic sort with a worst-case O(n log n) bound and O(1)
extra space, which makes it the fallback stage of introsort. Implementing it
teaches the implicit binary-tree encoding of an array, why bottom-up heap
construction is linear, and how the sorted suffix and heap prefix share one
buffer without conflict.

Status: scaffold — the source is a gate stub; tests assert the unimplemented
state.
