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

- `set`, `get`, `remove`, `contains`: expected O(1), O(n) under collisions
- `size`, `is_empty`: O(1)

## Benchmarking

Run the reusable benchmark harness from the repository root:

```bash
make benchmark NAME=data-structures/associative/hash-tables/separate-chaining BENCHMARK=hash_table
make benchmark NAME=data-structures/associative/hash-tables/separate-chaining BENCHMARK=hash_table BENCHMARK_ITEM_COUNT=100000
```

It measures `set`, `get`, `contains`, and `remove` across 21 timed samples,
excluding table setup, verification, teardown, and process startup from the
timed region. Results report minimum, median, and maximum nanoseconds per
operation. Compare multiple item counts to observe growth trends rather than
treating one machine's timing as proof of complexity.

The implementation has 10 fixed buckets and does not resize or rehash. It is
collision-correct, but its average chain length grows as entries are added, so
average operation cost grows with n. Expected O(1) behavior at scale depends
on load-factor resizing and rehashing, which this module does not perform.

## Implementation Notes

Use separate chaining: `buckets` is an array of `HashTableEntry *`, and every
entry points to the next entry in its collision chain. A bucket is selected by:

```c
size_t index = table->hash(key) % table->capacity;
```

The entry stores the original caller-owned key and value pointers, not the hash
result. Different keys may have the same hash, so lookup and mutation must
traverse the selected chain and call `table->equals(current->key, key)` before
treating two keys as the same.

For `set`, replace only the value when an equal key already exists; retain the
first key pointer as required by the contract. For a new key, allocate one entry
and prepend it to the selected bucket chain. `remove` uses `previous` and
`current` pointers to unlink the matched entry, frees that entry only, and never
frees caller-owned keys or values.

The generic singly linked-list module is intentionally not reused here. Hash
table buckets need direct entry traversal and key comparison, while the generic
list API only exposes index-based operations and would add unnecessary wrapper
nodes. A rehash must allocate a new bucket array and place every existing
entry using its stored key and the new capacity; do not cache a separate hash
value unless the contract and invariant explicitly require it.
