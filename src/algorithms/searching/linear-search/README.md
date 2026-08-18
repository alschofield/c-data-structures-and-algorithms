# Linear Search

Sequential scan that finds a target by comparing every element in order until a
match is found or the input is exhausted.

## Required API

```c
typedef int (*LinearSearchCompareFn)(const void *left, const void *right);

bool linear_search(void *const *items, size_t count, const void *key,
                   LinearSearchCompareFn compare, size_t *out_index);
```

The checked-in source is still the scaffold gate `bool
linear_search_is_implemented(void)`, which returns `false`; the test asserts exactly that.

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
