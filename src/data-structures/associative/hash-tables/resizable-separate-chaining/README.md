# Resizable Separate-Chaining Hash Table

Generic key-value collection that keeps collision chains short by growing and
rehashing its bucket array as entries are added. Keys and values are
caller-owned pointers; values may be `NULL`, but keys may not.

## How It Works

Hash the key to choose a bucket, then store collisions in that bucket's linked
chain. Before an insertion would push the load factor above 0.75, allocate a
bucket array with double the current capacity and rehash every existing entry:
the old bucket index is no longer valid because the calculation uses the new
capacity. Rehashing is expensive once, but capacity doubling makes that cost
amortized across many inserts, preserving O(1) expected operations as the
table grows.

## Required API

```c
typedef struct ResizableHashTable ResizableHashTable;
typedef size_t (*ResizableHashTableHashFn)(const void *key);
typedef bool (*ResizableHashTableEqualsFn)(const void *left, const void *right);

ResizableHashTable *resizable_hash_table_create(
    ResizableHashTableHashFn hash,
    ResizableHashTableEqualsFn equals
);
void resizable_hash_table_destroy(ResizableHashTable *table);
bool resizable_hash_table_set(
    ResizableHashTable *table,
    void *key,
    void *value,
    void **out_previous_value
);
bool resizable_hash_table_get(
    const ResizableHashTable *table,
    const void *key,
    void **out_value
);
bool resizable_hash_table_remove(
    ResizableHashTable *table,
    const void *key,
    void **out_value
);
bool resizable_hash_table_contains(const ResizableHashTable *table, const void *key);
size_t resizable_hash_table_size(const ResizableHashTable *table);
size_t resizable_hash_table_capacity(const ResizableHashTable *table);
bool resizable_hash_table_is_empty(const ResizableHashTable *table);
```

## Contract

- `create` requires non-`NULL` hash and equality functions and starts with 10
  zeroed buckets.
- `set` inserts a new key or replaces the value for an equal key. It returns
  the old value through `out_previous_value`, or `NULL` for a new key.
- Replacing a value retains the first stored key pointer; the caller retains
  ownership of a replacement key pointer.
- `get` and `remove` fail for absent or `NULL` keys without changing outputs.
- Entries remain correct when different keys have identical hash values.
- Before an insertion would make `size / capacity` exceed 0.75, capacity
  doubles and every entry is rehashed using `hash(key) % new_capacity`.
- If allocating the new bucket array fails, `set` returns `false` and leaves
  the table, its capacity, and `out_previous_value` unchanged.
- The table does not shrink automatically after removals.
- `destroy` frees table-owned buckets and entries, never keys or values.

## Complexity Targets

- `set`, `get`, `remove`, `contains`: O(1) expected amortized; O(n) worst case
- Resize/rehash: O(n), amortized across insertions
- `size`, `capacity`, `is_empty`: O(1)
- Space: O(bucket capacity + entries)
