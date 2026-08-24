# Hash Table

Generic key-value collection. Keys and values are caller-owned pointers; values
may be `NULL`, but keys may not.

## How It Works

Skip the search by computing the location: hash the key to pick one of the
buckets, then deal with collisions by letting each bucket hold a linked chain
of entries. Lookup hashes, jumps to the bucket, and walks the chain comparing
keys. `hash_table_set` keeps capacity fixed for collision-chain learning;
`hash_table_set_resizing` doubles and rehashes buckets before load factor
exceeds 0.75, keeping chains short for production-style lookup.

## Required API

```c
typedef struct HashTable HashTable;
typedef size_t (*HashTableHashFn)(const void *key);
typedef bool (*HashTableEqualsFn)(const void *left, const void *right);

HashTable *hash_table_create(HashTableHashFn hash, HashTableEqualsFn equals,
                             size_t initial_capacity);
void hash_table_destroy(HashTable *table);
bool hash_table_set(HashTable *table, void *key, void *value, void **out_previous_value);
bool hash_table_set_resizing(HashTable *table, void *key, void *value,
                             void **out_previous_value);
bool hash_table_get(const HashTable *table, const void *key, void **out_value);
bool hash_table_remove(HashTable *table, const void *key, void **out_value);
bool hash_table_contains(const HashTable *table, const void *key);
size_t hash_table_size(const HashTable *table);
size_t hash_table_capacity(const HashTable *table);
bool hash_table_is_empty(const HashTable *table);
```

## Contract

- `create` requires non-`NULL` hash/equality functions and a nonzero initial
  capacity. Existing callers pass `10U` for the standard starting capacity.
- Both set functions insert a new key or replace the value for an equal key.
  They return the old value through `out_previous_value`, or `NULL` for a new
  key.
- `hash_table_set` never changes capacity; it preserves the fixed-bucket
  collision behavior used by the original exercise and benchmark.
- `hash_table_set_resizing` checks whether adding a new key would exceed a
  0.75 load factor. If so, it doubles capacity, allocates new buckets, and
  rehashes every entry using `hash(key) % new_capacity` before insertion.
- If resize allocation fails, `hash_table_set_resizing` returns `false` and
  leaves the table and `out_previous_value` unchanged.
- Replacing a value retains the first stored key pointer; the caller retains
  ownership of a replacement key pointer.
- `get` and `remove` fail for absent or `NULL` keys without changing outputs.
- Entries must remain correct when different keys have identical hash values.
- `destroy` frees table-owned buckets and entries, never keys or values.

## Complexity Targets

- `hash_table_set`, `get`, `remove`, `contains`: expected O(1) with short
  chains, O(n / buckets) as fixed-bucket chains grow, O(n) worst case
- `hash_table_set_resizing`: expected O(1) amortized; O(n) for a resize
- `size`, `capacity`, `is_empty`: O(1)
- Space: O(entries + capacity)
