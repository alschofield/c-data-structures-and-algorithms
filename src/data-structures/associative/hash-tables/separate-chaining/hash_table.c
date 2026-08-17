// Provides the public HashTable type and function declarations.
#include "hash_table.h"
// Declares malloc, calloc, and free.
#include <stdlib.h>

// Defines one key-value entry in a bucket's collision chain.
struct HashTableEntry {
    // Stores the caller-owned key pointer.
    void *key;
    // Stores the caller-owned value pointer.
    void *value;
    // Points to the next entry with the same bucket index.
    HashTableEntry *next;
};

// Defines the fields hidden from callers of the public API.
struct HashTable {
    // Counts key-value entries across every bucket chain.
    size_t size;
    // Counts pointer slots in the bucket array.
    size_t capacity;
    // Converts a caller-owned key pointer into a size_t hash value.
    HashTableHashFn hash;
    // Compares two caller-owned keys for logical equality.
    HashTableEqualsFn equals;
    // Points to the dynamically allocated array of bucket-chain heads.
    HashTableEntry **buckets;
};

// Allocates an empty table with caller-supplied key behavior.
HashTable *hash_table_create(HashTableHashFn hash, HashTableEqualsFn equals) {
    // Rejects a missing key-to-hash function.
    if (hash == NULL) {
        // Reports invalid configuration to the caller.
        return NULL;
    }

    // Rejects a missing key-equality function.
    if (equals == NULL) {
        // Reports invalid configuration to the caller.
        return NULL;
    }

    // Allocates the table wrapper before allocating its bucket array.
    HashTable *table = malloc(sizeof(HashTable));
    // Stops before accessing the wrapper when allocation fails.
    if (table == NULL) {
        // Reports allocation failure to the caller.
        return NULL;
    }

    // Allocates ten zeroed bucket-head pointers for empty collision chains.
    table->buckets = calloc(10U, sizeof(*table->buckets));
    // Checks whether bucket-array allocation failed.
    if (table->buckets == NULL) {
        // Releases the already-allocated table wrapper.
        free(table);
        // Reports allocation failure to the caller.
        return NULL;
    }

    // Records that no key-value entries have been inserted yet.
    table->size = 0U;
    // Records the fixed initial bucket-array capacity.
    table->capacity = 10U;
    // Stores the caller's key-to-hash function pointer.
    table->hash = hash;
    // Stores the caller's key-equality function pointer.
    table->equals = equals;
    // Returns the fully initialized empty table.
    return table;
}

// Frees bucket chains, the bucket array, and the table wrapper.
void hash_table_destroy(HashTable *table) {
    // Makes destruction of a null table a safe no-op.
    if (table == NULL) {
        // Returns because there is no allocation to release.
        return;
    }

    // Visits every collision-chain head in the bucket array.
    for (size_t i = 0; i < table->capacity; i++) {
        // Starts at the first entry stored in this bucket.
        HashTableEntry *temp = table->buckets[i];
        // Releases every entry in this bucket's collision chain.
        while (temp != NULL) {
            // Saves the chain link before the current entry becomes invalid.
            HashTableEntry *next = temp->next;
            // Releases the table-owned entry but not caller-owned key/value pointers.
            free(temp);
            // Continues with the next entry in this collision chain.
            temp = next;
        }
    }

    // Releases the array that held the collision-chain heads.
    free(table->buckets);
    // Frees the HashTable structure.
    free(table);
}

// Inserts a new key-value pair or replaces an existing key's value.
bool hash_table_set(HashTable *table, void *key, void *value, void **out_previous_value) {
    // Rejects a missing table pointer.
    if (table == NULL) {
        // Reports failure because there is no table to modify.
        return false;
    }

    // Rejects null keys because this table's contract disallows them.
    if (key == NULL) {
        // Leaves the table and output pointer unchanged.
        return false;
    }

    // Rejects a missing output location.
    if (out_previous_value == NULL) {
        // Reports failure without inserting or replacing an entry.
        return false;
    }

    // Starts at the collision-chain head selected by this key's hash.
    HashTableEntry *temp = table->buckets[table->hash(key) % table->capacity];
    // Searches existing entries for an equal key before inserting a new entry.
    while(temp != NULL) {
        if (table->equals(temp->key, key)) {
            // Returns the replaced caller-owned value to the caller.
            *out_previous_value = temp->value;
            // Retains the first stored key pointer and replaces only its value.
            temp->value = value;
            // Reports that the existing entry was replaced.
            return true;
        }

        // Advances until an equal key is found or the chain ends.
        temp = temp->next;
    }

    // Allocates a new entry after confirming no equal key exists.
    HashTableEntry *new_entry = malloc(sizeof(HashTableEntry));
    if (new_entry == NULL) {
        // Leaves the table and output pointer unchanged on allocation failure.
        return false;
    }

    // Stores the caller-owned key and value pointers in the new entry.
    new_entry->key = key;
    new_entry->value = value;
    // Prepends the entry because collision-chain order has no meaning.
    new_entry->next = table->buckets[table->hash(key) % table->capacity];
    // Makes the new entry the selected bucket's collision-chain head.
    table->buckets[table->hash(key) % table->capacity] = new_entry;
    // Reports that no prior value existed for the newly inserted key.
    *out_previous_value = NULL;
    // Counts the newly inserted key-value entry.
    table->size++;
    // Reports successful insertion.
    return true;
}

// Looks up a value by a caller-supplied key.
bool hash_table_get(const HashTable *table, const void *key, void **out_value) {
    // Rejects a missing table pointer.
    if (table == NULL) {
        // Reports failure because there is no table to inspect.
        return false;
    }

    // Rejects null keys because this table's contract disallows them.
    if (key == NULL) {
        // Leaves the caller's output pointer unchanged.
        return false;
    }

    // Rejects a missing output location.
    if (out_value == NULL) {
        // Reports failure without changing table state.
        return false;
    }

    // Starts at the collision-chain head selected by this key's hash.
    HashTableEntry *temp = table->buckets[table->hash(key) % table->capacity];
    // Searches the collision chain for an equal key.
    while(temp != NULL) {
        if (table->equals(temp->key, key)) {
            // Copies the stored caller-owned value pointer to the output location.
            *out_value = temp->value;
            // Reports the successful lookup immediately.
            return true;
        }

        // Advances until an equal key is found or the chain ends.
        temp = temp->next;
    }

    // Reports absence after the collision chain contains no matching key.
    return false;
}

// Removes a key-value pair and returns its value through out_value.
bool hash_table_remove(HashTable *table, const void *key, void **out_value) {
    // Rejects a missing table pointer.
    if (table == NULL) {
        // Reports failure because there is no table to modify.
        return false;
    }

    // Rejects null keys because this table's contract disallows them.
    if (key == NULL) {
        // Leaves the table and caller's output pointer unchanged.
        return false;
    }

    // Rejects a missing output location.
    if (out_value == NULL) {
        // Reports failure without removing an entry.
        return false;
    }

    // Starts at the collision-chain head selected by this key's hash.
    HashTableEntry *current = table->buckets[table->hash(key) % table->capacity];
    // Records the entry before current so a match can be unlinked.
    HashTableEntry *previous = NULL;

    // Searches the collision chain for an equal key.
    while (current != NULL) {
        if (table->equals(current->key, key)) {
            // Returns the stored caller-owned value before releasing the entry.
            *out_value = current->value;

            if (previous == NULL) {
                // Replaces the bucket head when removing its first entry.
                table->buckets[table->hash(key) % table->capacity] = current->next;
            } else {
                // Bypasses a matched entry that follows another entry.
                previous->next = current->next;
            }

            // Releases the table-owned entry but not its key or value.
            free(current);
            // Keeps the tracked entry count accurate after removal.
            table->size--;
            // Reports that the key was removed.
            return true;
        }

        // Advances both pointers while retaining the predecessor link.
        previous = current;
        current = current->next;
    }

    // Reports absence without changing the caller's output pointer.
    return false;
}

// Reports whether a caller-supplied key is present.
bool hash_table_contains(const HashTable *table, const void *key) {
    // Rejects a missing table pointer.
    if (table == NULL) {
        // Reports that no key can be present.
        return false;
    }

    // Rejects null keys because this table's contract disallows them.
    if (key == NULL) {
        // Reports that no null key can be present.
        return false;
    }

    // Starts at the collision-chain head selected by this key's hash.
    HashTableEntry *temp = table->buckets[table->hash(key) % table->capacity];
    // Searches the collision chain for an equal key.
    while(temp != NULL) {
        if (table->equals(temp->key, key)) {
            // Reports presence as soon as an equal key is found.
            return true;
        }

        // Advances until an equal key is found or the chain ends.
        temp = temp->next;
    }

    // Reports absence after the collision chain contains no matching key.
    return false;
}

// Returns the tracked number of entries without traversing bucket chains.
size_t hash_table_size(const HashTable *table) {
    // Treats a null table as containing zero entries.
    if (table == NULL) {
        // Returns the empty-table count.
        return 0U;
    }

    // Returns the constant-time entry count.
    return table->size;
}

// Reports whether a table contains no entries.
bool hash_table_is_empty(const HashTable *table) {
    // Treats a null table as empty under this API contract.
    if (table == NULL) {
        // Reports an empty table.
        return true;
    }

    // Uses the constant-time entry count to report emptiness.
    return table->size == 0U;
}
