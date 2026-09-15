# Bubble Sort

Repeatedly swaps adjacent out-of-order item pointers, shrinking the unsorted suffix after each pass. A pass with no swaps ends early.

## C API
```c
typedef int (*BubbleSortCompareFn)(const void *left, const void *right);
bool bubble_sort(void **items, size_t count, BubbleSortCompareFn compare);
```

## Contract
- Sorts `items` in ascending callback order in place and is stable because equal items are never swapped.
- `compare` is required even for `count == 0`; an empty array succeeds without dereferencing `items`. A nonempty null array fails.
- The function reorders only caller-owned pointer slots; it neither allocates nor owns or frees items. A failed validation leaves the array unchanged.

## Complexity and Verification
Best O(n); average and worst O(n^2); O(1) auxiliary space. Verify with `make test NAME=algorithms/sorting/comparison/bubble-sort`.

Comparison sort that repeatedly sweeps the array, swapping adjacent
out-of-order pairs until a full pass makes no swaps.

## How It Works

Repeated neighborhood sweeps. Walk the array comparing adjacent pairs and
swapping any pair that is out of order; each full pass carries the largest
remaining element to the end of the unsorted region like a bubble rising.
The shrinking boundary exploits that: after pass k the last k slots are
final, so the next pass stops earlier. A pass with zero swaps proves the
array is sorted, which is the required early exit and the O(n) best case.
Swapping only on strictly-greater keeps equal elements in their original
order (stable).

## Required API

```c
typedef int (*BubbleSortCompareFn)(const void *left, const void *right);

bool bubble_sort(void **items, size_t count, BubbleSortCompareFn compare);
```

## Contract

- Sorts in place into ascending order under the caller's comparison.
- Stable: equal elements keep their original relative order (adjacent swaps
  only, and never swap on equality).
- Must implement the early-exit optimization: a pass with zero swaps
  terminates the sort.
- Zero-length and single-element inputs are no-ops; a `NULL` array with
  nonzero length is invalid input handled without undefined behavior.
- After each pass `k`, the largest `k` elements occupy their final positions
  at the tail.

## Complexity Targets

- Best: O(n) (already sorted input, early exit after one pass)
- Average: O(n^2)
- Worst: O(n^2) (reverse-sorted input)
- Space: O(1), in place
