# Merge Sort

Divide-and-conquer comparison sort that recursively sorts halves and merges
them with an auxiliary buffer.

## How It Works

Divide and conquer. Split the array in half, sort each half (recursively,
down to single elements, which are trivially sorted), then merge: walk both
sorted halves front-to-front, repeatedly taking the smaller head into the
output. The merge is where the work and the guarantees live — taking from the
left run on ties is what makes the sort stable, and no input order can make
merging degrade, which is why the cost is O(n log n) unconditionally. The
price is the O(n) auxiliary buffer the merge writes into.

## Required API

```c
typedef int (*MergeSortCompareFn)(const void *left, const void *right);

bool merge_sort(void **items, size_t count, MergeSortCompareFn compare);
```

## Contract

- Sorts into ascending order under the caller's comparison.
- Stable: on ties the merge step must take from the left run first.
- Guaranteed O(n log n) regardless of input order; no adversarial input
  degrades it.
- Requires an O(n) auxiliary buffer; allocation failure must be reported
  cleanly and leave the input unmodified.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- The merge must handle uneven halves and runs that exhaust at different
  times without reading out of bounds.

## Complexity Targets

- Best: O(n log n)
- Average: O(n log n)
- Worst: O(n log n)
- Space: O(n) auxiliary buffer (plus O(log n) recursion depth)
