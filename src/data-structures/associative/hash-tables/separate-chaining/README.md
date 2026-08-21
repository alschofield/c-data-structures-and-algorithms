# Hash Table

Generic key-value collection. Keys and values are caller-owned pointers; values
may be `NULL`, but keys may not.

## How It Works

Skip the search by computing the location: hash the key to pick one of the
buckets, then deal with collisions by letting each bucket hold a linked
chain of entries. Lookup hashes, jumps to the bucket, and walks the chain
comparing keys. With enough buckets the chains stay short and every
operation is O(1) expected; this implementation's 10 fixed buckets make the
chains grow with n instead, degrading to O(n/buckets) — measured honestly
in the benchmarks as the cost of skipping rehashing.

## Required API

```c
typedef struct HashTable HashTable;
typedef size_t (*HashTableHashFn)(const void *key);
typedef bool (*HashTableEqualsFn)(const void *left, const void *right);

HashTable *hash_table_create(HashTableHashFn hash, HashTableEqualsFn equals);
void hash_table_destroy(HashTable *table);
bool hash_table_set(HashTable *table, void *key, void *value, void **out_previous_value);
bool hash_table_get(const HashTable *table, const void *key, void **out_value);
bool hash_table_remove(HashTable *table, const void *key, void **out_value);
bool hash_table_contains(const HashTable *table, const void *key);
size_t hash_table_size(const HashTable *table);
bool hash_table_is_empty(const HashTable *table);
```

## Contract

- `create` requires non-`NULL` hash and equality functions.
- `set` inserts a new key or replaces the value for an equal key. It returns the
  old value through `out_previous_value`, or `NULL` for a new key.
- Replacing a value retains the first stored key pointer; the caller retains
  ownership of a replacement key pointer.
- `get` and `remove` fail for absent or `NULL` keys without changing outputs.
- Entries must remain correct when different keys have identical hash values.
- `destroy` frees table-owned buckets and entries, never keys or values.

## Complexity Targets

- `set`, `get`, `remove`, `contains`: expected O(1) with short chains,
  O(n / buckets) as fixed-bucket chains grow, O(n) worst case
- `size`, `is_empty`: O(1)
- Space: O(entries + 10 buckets)
