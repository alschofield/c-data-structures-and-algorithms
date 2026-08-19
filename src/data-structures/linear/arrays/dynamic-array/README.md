# Dynamic Array

Generic resizable contiguous collection of caller-owned `void *` values,
including `NULL`.

## How It Works

A plain array that lies about its size. It keeps capacity (allocated) apart
from size (used); append writes at index size and increments. When capacity
runs out, allocate roughly double, copy everything once, and continue. The
doubling is why append is O(1) amortized: an occasional O(n) copy is paid
for by the n cheap appends that preceded it. Indexing is pure arithmetic,
which is also why the structure is the cache-friendliest thing here.

## Required API

```c
typedef struct DynamicArray DynamicArray;

DynamicArray *dynamic_array_create(void);
void dynamic_array_destroy(DynamicArray *array);
bool dynamic_array_get(const DynamicArray *array, size_t index, void **out_item);
bool dynamic_array_set(DynamicArray *array, size_t index, void *item, void **out_old_item);
bool dynamic_array_insert(DynamicArray *array, size_t index, void *item);
bool dynamic_array_remove(DynamicArray *array, size_t index, void **out_item);
size_t dynamic_array_size(const DynamicArray *array);
size_t dynamic_array_capacity(const DynamicArray *array);
bool dynamic_array_is_empty(const DynamicArray *array);
```

## Contract

- Valid element indexes are `[0, size)`; `insert` also accepts `size` to append.
- `set` returns the replaced value through `out_old_item`; `remove` returns and
  removes the indexed value.
- Failed access/mutation operations leave output parameters and contents unchanged.
- Capacity grows geometrically when required; capacity is always at least size.
- `destroy` frees only array-owned storage.

## Complexity Targets

- `get`, `set`, `size`, `capacity`, `is_empty`: O(1)
- `insert` at `size`: amortized O(1)
- Other `insert` operations and `remove`: O(n)
