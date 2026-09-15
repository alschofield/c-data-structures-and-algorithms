# Separate-Chaining Hash Table

An opaque hash table whose buckets hold linked collision chains. `set` keeps fixed capacity; `set_resizing` doubles and rehashes before a new insertion at approximately 75% load.

## C API
```c
typedef size_t (*HashTableHashFn)(const void *key); typedef bool (*HashTableEqualsFn)(const void *left, const void *right);
HashTable *hash_table_create(HashTableHashFn hash, HashTableEqualsFn equals, size_t initial_capacity); void hash_table_destroy(HashTable *table);
bool hash_table_set(HashTable *table, void *key, void *value, void **out_previous_value);
bool hash_table_set_resizing(HashTable *table, void *key, void *value, void **out_previous_value);
bool hash_table_get(const HashTable *table, const void *key, void **out_value); bool hash_table_remove(HashTable *table, const void *key, void **out_value);
bool hash_table_contains(const HashTable *table, const void *key); size_t hash_table_size(const HashTable *table); size_t hash_table_capacity(const HashTable *table); bool hash_table_is_empty(const HashTable *table);
```

## Behavior, Ownership, and Invariants
- Creation requires non-null callbacks and nonzero, byte-addressable capacity. Keys may not be null; values may be null. Hash and equality must be consistent and valid for the table lifetime.
- Updates retain the original stored key and return its old value. New keys set `*out_previous_value` to null. Get/remove outputs are written only on success.
- The table owns buckets and entries, not keys or values; destroy is null-safe. Resize arithmetic/allocation failures preserve the logical table. Null queries yield size/capacity zero and empty true.

## Complexity and Verification
Expected set/get/remove O(1), worst O(n); resizing is O(n); space is O(capacity + entries). Verify with `make test NAME=data-structures/associative/hash-tables/separate-chaining`.
