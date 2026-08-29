# Linear Search

Sequential scan that finds a target by comparing every element in order until a
match is found or the input is exhausted.

## How It Works

The honest baseline: examine elements front to back until one matches or the
input is exhausted. No ordering precondition, no preprocessing — this is the
only search that works on arbitrary unsorted data. The contract wrinkle is
duplicates: the scan direction guarantees the first match is the one
reported.

## Required API

```c
typedef int (*LinearSearchCompareFn)(const void *left, const void *right);

bool linear_search(void *const *items, size_t count, const void *key,
                   LinearSearchCompareFn compare, size_t *out_index);
```

The checked-in implementation performs a first-match sequential scan without
modifying caller-owned items.

## Contract

- Works on unsorted input; no ordering precondition may be assumed.
- Returns the index of the first matching element when duplicates exist.
- A missing target is reported distinctly (no valid index), never by returning
  an arbitrary position.
- An empty input or `NULL` array pointer is a clean not-found result, not
  undefined behavior.
- The input array is never modified.

## Complexity Targets

- Best: O(1) (target at index 0)
- Average: O(n)
- Worst: O(n) (target absent or last)
- Space: O(1)

## Verification

```text
make test NAME=algorithms/searching/linear-search
make benchmark NAME=algorithms/searching/linear-search BENCHMARK=linear_search
```

At 2,000 items per sample on this development machine, measured medians were
3.40 ns/op for a first-item match, 1,016.35 ns/op for a middle match,
2,007.30 ns/op for a last-item match, and 2,050.10 ns/op for a missing key.
