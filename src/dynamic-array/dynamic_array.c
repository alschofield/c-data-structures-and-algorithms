#include "dynamic_array.h"
#include <stdlib.h>

struct DynamicArray {
    size_t capacity;
    size_t size;
    void **items;
};

DynamicArray *dynamic_array_create(void) {
    DynamicArray *array = malloc(sizeof(DynamicArray));

    if (array == NULL) {
        return NULL;
    }

    array->capacity = 0U;
    array->size = 0U;
    array->items = NULL;

    return array;
}

void dynamic_array_destroy(DynamicArray *array) {
    if (array == NULL) {
        return;
    }

    free(array->items);
    free(array);

    return;
}

bool dynamic_array_get(const DynamicArray *array, size_t index, void **out_item) {
    if (array == NULL) {
        return false;
    }

    if (out_item == NULL) {
        return false;
    }

    if (index >= array->size) {
        return false;
    }

    *out_item = array->items[index];

    return true;
}

bool dynamic_array_set(DynamicArray *array, size_t index, void *item, void **out_old_item) {
    if (array == NULL) {
        return false;
    }

    if (out_old_item == NULL) {
        return false;
    }

    if (index >= array->size) {
        return false;
    }

    *out_old_item = array->items[index];
    array->items[index] = item;

    return true;
}

bool dynamic_array_insert(DynamicArray *array, size_t index, void *item) {
    if (array == NULL) {
        return false;
    }

    if (index > array->size) {
        return false;
    }

    if (array->size == array->capacity) {
        size_t new_capacity = array->capacity == 0U ? 2U : array->capacity * 2U;
        void **items = realloc(array->items, new_capacity * sizeof(*array->items));
        if (items == NULL) {
            return false;
        }

        array->items = items;
        array->capacity = new_capacity;
    }

    for (size_t i = array->size; i > index; i--) {
        array->items[i] = array->items[i - 1U];
    }

    array->items[index] = item;
    array->size++;

    return true;
}

bool dynamic_array_remove(DynamicArray *array, size_t index, void **out_item) {
    if (array == NULL) {
        return false;
    }

    if (index >= array->size) {
        return false;
    }

    if (out_item == NULL) {
        return false;
    }

    *out_item = array->items[index];

    for (size_t i = index; i < (array->size - 1U); i++) {
        array->items[i] = array->items[i + 1U];
    }

    array->items[array->size - 1U] = NULL;
    array->size--;

    return true;
}

size_t dynamic_array_size(const DynamicArray *array) {
    if (array == NULL) {
        return 0U;
    }

    return array->size;
}

size_t dynamic_array_capacity(const DynamicArray *array) {
    if (array == NULL) {
        return 0U;
    }

    return array->capacity;
}

bool dynamic_array_is_empty(const DynamicArray *array) {
    if (array == NULL) {
        return true;
    }

    return array->size == 0U;
}
