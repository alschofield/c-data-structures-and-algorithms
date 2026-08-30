// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Names the opaque separate-chaining hash table.
typedef struct HashTable HashTable;
// Names the private chained-entry type.
typedef struct HashTableEntry HashTableEntry;
// Hashes one caller-owned key into an unsigned value.
typedef size_t (*HashTableHashFn)(const void *key);
// Reports whether two caller-owned keys are equal.
typedef bool (*HashTableEqualsFn)(const void *left, const void *right);

// Allocates a hash table with fixed or resizable bucket policy chosen by setters.
HashTable *hash_table_create(HashTableHashFn hash, HashTableEqualsFn equals,
                              size_t initial_capacity);
// Frees table entries and buckets without freeing caller-owned keys or values.
void hash_table_destroy(HashTable *table);
// Sets one key/value pair without resizing the bucket array.
bool hash_table_set(HashTable *table, void *key, void *value, void **out_previous_value);
// Sets one key/value pair and grows the bucket array when required.
bool hash_table_set_resizing(HashTable *table, void *key, void *value,
                              void **out_previous_value);
// Returns one stored value by key.
bool hash_table_get(const HashTable *table, const void *key, void **out_value);
// Removes and returns one stored value by key.
bool hash_table_remove(HashTable *table, const void *key, void **out_value);
// Reports whether key is stored in the table.
bool hash_table_contains(const HashTable *table, const void *key);
// Returns the number of stored key/value pairs.
size_t hash_table_size(const HashTable *table);
// Returns the current bucket-array capacity.
size_t hash_table_capacity(const HashTable *table);
// Reports whether the table contains no entries.
bool hash_table_is_empty(const HashTable *table);
