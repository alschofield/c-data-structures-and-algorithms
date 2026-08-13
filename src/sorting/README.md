# Sorting

Generic in-place sorting of caller-owned pointers using a caller-supplied
comparator.

## Required API

```c
typedef int (*SortCompareFn)(const void *left, const void *right);

bool bubble_sort(void **items, size_t count, SortCompareFn compare);
bool insertion_sort(void **items, size_t count, SortCompareFn compare);
bool selection_sort(void **items, size_t count, SortCompareFn compare);
bool merge_sort(void **items, size_t count, SortCompareFn compare);
bool quick_sort(void **items, size_t count, SortCompareFn compare);
```

## Contract

- Each algorithm sorts `items` into ascending comparator order in place.
- `items == NULL` is valid only when `count == 0U`.
- `compare` is required whenever `count > 1U`.
- Algorithms rearrange pointer slots only; they never free or mutate caller-owned
  values.
- A failed operation returns `false` without claiming a sorted result.

## Learning Sequence

1. Bubble sort: adjacent swaps and early-exit optimization.
2. Selection sort: minimum selection and swap minimization.
3. Insertion sort: sorted-prefix invariant and nearly-sorted input.
4. Merge sort: recursion, temporary storage, and stable merging.
5. Quick sort: partitions, pivots, recursion depth, and adversarial inputs.

## Complexity Targets

| Algorithm | Average | Worst | Extra space | Stable |
| --- | --- | --- | --- | --- |
| Bubble | O(n^2) | O(n^2) | O(1) | Yes |
| Selection | O(n^2) | O(n^2) | O(1) | No |
| Insertion | O(n^2) | O(n^2) | O(1) | Yes |
| Merge | O(n log n) | O(n log n) | O(n) | Yes |
| Quick | O(n log n) | O(n^2) | O(log n) typical | No |
