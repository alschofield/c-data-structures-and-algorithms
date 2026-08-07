#include "dynamic_array.h"

#include <assert.h>
#include <stddef.h>

struct Record {
    int id;
};

static void test_empty_and_invalid_access(void) {
    DynamicArray *array = dynamic_array_create();
    int sentinel = 0;
    void *out = &sentinel;

    assert(array != NULL);
    assert(dynamic_array_size(array) == 0U);
    assert(dynamic_array_is_empty(array));
    assert(!dynamic_array_get(array, 0U, &out));
    assert(out == &sentinel);
    assert(!dynamic_array_set(array, 0U, NULL, &out));
    assert(out == &sentinel);
    assert(!dynamic_array_remove(array, 0U, &out));
    assert(out == &sentinel);
    assert(!dynamic_array_get(array, 0U, NULL));
    assert(!dynamic_array_set(array, 0U, NULL, NULL));
    assert(!dynamic_array_remove(array, 0U, NULL));
    dynamic_array_destroy(array);
}

static void test_indexed_operations_and_generic_values(void) {
    DynamicArray *array = dynamic_array_create();
    int first = 1;
    int second = 2;
    int replacement = 3;
    struct Record record = { .id = 4 };
    void *out = NULL;

    assert(dynamic_array_push(array, &first));
    assert(dynamic_array_push(array, &second));
    assert(dynamic_array_insert(array, 1U, &record));
    assert(dynamic_array_insert(array, 3U, NULL));
    assert(!dynamic_array_insert(array, 5U, &first));
    assert(dynamic_array_size(array) == 4U);

    assert(dynamic_array_get(array, 0U, &out));
    assert(out == &first);
    assert(dynamic_array_get(array, 1U, &out));
    assert(out == &record);
    assert(dynamic_array_get(array, 2U, &out));
    assert(out == &second);
    assert(dynamic_array_get(array, 3U, &out));
    assert(out == NULL);

    assert(dynamic_array_set(array, 2U, &replacement, &out));
    assert(out == &second);
    assert(dynamic_array_remove(array, 1U, &out));
    assert(out == &record);
    assert(dynamic_array_get(array, 1U, &out));
    assert(out == &replacement);
    assert(dynamic_array_remove(array, 2U, &out));
    assert(out == NULL);
    dynamic_array_destroy(array);
}

static void test_growth(void) {
    enum { ITEM_COUNT = 1024 };
    DynamicArray *array = dynamic_array_create();
    int values[ITEM_COUNT];
    size_t initial_capacity = dynamic_array_capacity(array);
    void *out = NULL;

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        values[index] = (int)index;
        assert(dynamic_array_push(array, &values[index]));
    }
    assert(dynamic_array_size(array) == ITEM_COUNT);
    assert(dynamic_array_capacity(array) >= ITEM_COUNT);
    assert(dynamic_array_capacity(array) > initial_capacity);
    assert(dynamic_array_get(array, ITEM_COUNT - 1U, &out));
    assert(out == &values[ITEM_COUNT - 1U]);
    dynamic_array_destroy(array);
}

static void test_null_container(void) {
    int value = 1;
    void *out = &value;

    assert(!dynamic_array_push(NULL, &value));
    assert(!dynamic_array_get(NULL, 0U, &out));
    assert(!dynamic_array_set(NULL, 0U, &value, &out));
    assert(!dynamic_array_insert(NULL, 0U, &value));
    assert(!dynamic_array_remove(NULL, 0U, &out));
    assert(out == &value);
    assert(dynamic_array_size(NULL) == 0U);
    assert(dynamic_array_capacity(NULL) == 0U);
    assert(dynamic_array_is_empty(NULL));
    dynamic_array_destroy(NULL);
}

int main(void) {
    test_empty_and_invalid_access();
    test_indexed_operations_and_generic_values();
    test_growth();
    test_null_container();
    return 0;
}
