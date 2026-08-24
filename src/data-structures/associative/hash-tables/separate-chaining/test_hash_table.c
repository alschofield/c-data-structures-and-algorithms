#include "hash_table.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

struct Key {
    int id;
};

static size_t key_hash(const void *key) {
    const struct Key *typed_key = key;

    return (size_t)typed_key->id;
}

static size_t constant_hash(const void *key) {
    (void)key;
    return 0U;
}

static bool key_equals(const void *left, const void *right) {
    const struct Key *left_key = left;
    const struct Key *right_key = right;

    return left_key->id == right_key->id;
}

static void test_creation_and_empty_table(void) {
    HashTable *table;
    struct Key key = { .id = 1 };
    int sentinel = 0;
    void *out = &sentinel;

    assert(hash_table_create(NULL, key_equals, 10U) == NULL);
    assert(hash_table_create(key_hash, NULL, 10U) == NULL);
    assert(hash_table_create(key_hash, key_equals, 0U) == NULL);
    table = hash_table_create(key_hash, key_equals, 10U);
    assert(table != NULL);
    assert(hash_table_size(table) == 0U);
    assert(hash_table_capacity(table) == 10U);
    assert(hash_table_is_empty(table));
    assert(!hash_table_get(table, &key, &out));
    assert(out == &sentinel);
    assert(!hash_table_remove(table, &key, &out));
    assert(out == &sentinel);
    assert(!hash_table_get(table, &key, NULL));
    assert(!hash_table_remove(table, &key, NULL));
    assert(!hash_table_contains(table, NULL));
    hash_table_destroy(table);
}

static void test_insert_replace_and_remove(void) {
    HashTable *table = hash_table_create(key_hash, key_equals, 10U);
    struct Key first_key = { .id = 1 };
    struct Key equal_first_key = { .id = 1 };
    struct Key second_key = { .id = 2 };
    int first_value = 10;
    int replacement_value = 11;
    void *out = NULL;

    assert(hash_table_set(table, &first_key, &first_value, &out));
    assert(out == NULL);
    assert(hash_table_set(table, &second_key, NULL, &out));
    assert(out == NULL);
    assert(hash_table_size(table) == 2U);
    assert(hash_table_contains(table, &equal_first_key));
    assert(hash_table_get(table, &equal_first_key, &out));
    assert(out == &first_value);
    assert(hash_table_get(table, &second_key, &out));
    assert(out == NULL);

    assert(hash_table_set(table, &equal_first_key, &replacement_value, &out));
    assert(out == &first_value);
    assert(hash_table_size(table) == 2U);
    assert(hash_table_get(table, &first_key, &out));
    assert(out == &replacement_value);
    assert(hash_table_remove(table, &equal_first_key, &out));
    assert(out == &replacement_value);
    assert(!hash_table_contains(table, &first_key));
    assert(hash_table_size(table) == 1U);
    assert(!hash_table_set(table, NULL, &first_value, &out));
    assert(!hash_table_get(table, NULL, &out));
    assert(!hash_table_remove(table, NULL, &out));
    hash_table_destroy(table);
}

static void test_collisions_with_fixed_capacity(void) {
    enum { ITEM_COUNT = 256 };
    HashTable *table = hash_table_create(constant_hash, key_equals, 10U);
    struct Key keys[ITEM_COUNT];
    int values[ITEM_COUNT];
    void *out = NULL;

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        keys[index].id = (int)index;
        values[index] = (int)(index * 2U);
        assert(hash_table_set(table, &keys[index], &values[index], &out));
        assert(out == NULL);
    }
    assert(hash_table_size(table) == ITEM_COUNT);
    assert(hash_table_capacity(table) == 10U);

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        assert(hash_table_get(table, &keys[index], &out));
        assert(out == &values[index]);
    }
    for (size_t index = 0U; index < ITEM_COUNT; index += 2U) {
        assert(hash_table_remove(table, &keys[index], &out));
        assert(out == &values[index]);
    }
    assert(hash_table_size(table) == ITEM_COUNT / 2U);
    for (size_t index = 1U; index < ITEM_COUNT; index += 2U) {
        assert(hash_table_get(table, &keys[index], &out));
        assert(out == &values[index]);
    }
    hash_table_destroy(table);
}

static void test_resizing_rehashes_every_entry(void) {
    enum { ITEM_COUNT = 8 };
    HashTable *table = hash_table_create(key_hash, key_equals, 10U);
    struct Key keys[ITEM_COUNT];
    int values[ITEM_COUNT];
    void *out = NULL;

    assert(table != NULL);
    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        keys[index].id = (int)index;
        values[index] = (int)(index * 10U);
        assert(hash_table_set_resizing(table, &keys[index], &values[index], &out));
        assert(out == NULL);
    }
    assert(hash_table_size(table) == ITEM_COUNT);
    assert(hash_table_capacity(table) == 20U);

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        assert(hash_table_get(table, &keys[index], &out));
        assert(out == &values[index]);
    }
    hash_table_destroy(table);
}

static void test_null_container(void) {
    struct Key key = { .id = 1 };
    int value = 1;
    void *out = &value;

    assert(!hash_table_set(NULL, &key, &value, &out));
    assert(!hash_table_get(NULL, &key, &out));
    assert(!hash_table_remove(NULL, &key, &out));
    assert(out == &value);
    assert(!hash_table_contains(NULL, &key));
    assert(hash_table_size(NULL) == 0U);
    assert(hash_table_capacity(NULL) == 0U);
    assert(hash_table_is_empty(NULL));
    hash_table_destroy(NULL);
}

int main(void) {
    test_creation_and_empty_table();
    test_insert_replace_and_remove();
    test_collisions_with_fixed_capacity();
    test_resizing_rehashes_every_entry();
    test_null_container();
    return 0;
}
