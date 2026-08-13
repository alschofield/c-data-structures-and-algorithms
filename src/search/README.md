# Search

Generic searches over caller-owned pointers using a caller-supplied comparator.

## Required API

```c
typedef int (*SearchCompareFn)(const void *left, const void *right);

bool linear_search(void *const *items, size_t count, const void *key,
    SearchCompareFn compare, size_t *out_index);
bool binary_search(void *const *items, size_t count, const void *key,
    SearchCompareFn compare, size_t *out_index);
```

## Contract

- Comparators follow `qsort` semantics: negative, zero, or positive.
- `linear_search` accepts unsorted input and returns the first matching index.
- `binary_search` requires ascending input under `compare`.
- A successful search writes the matching index to `out_index` and returns `true`.
- A failed operation returns `false` without changing `out_index`.
- Empty input is valid when `items == NULL` and `count == 0U`.

## Complexity Targets

- Linear search: O(n) time, O(1) extra space.
- Binary search: O(log n) time, O(1) extra space.

## Learning Focus

Binary search exposes overflow-safe midpoint calculation, loop invariants, and
the difference between an ordered representation and an arbitrary collection.
