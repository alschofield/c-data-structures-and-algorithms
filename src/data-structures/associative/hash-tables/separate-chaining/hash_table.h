#include <stdbool.h>
#include <stddef.h>

typedef struct HashTable HashTable;
typedef struct HashTableEntry HashTableEntry;
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
