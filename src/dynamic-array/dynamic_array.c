// Provides the public DynamicArray type and function declarations.
#include "dynamic_array.h"
// Declares malloc, realloc, and free.
#include <stdlib.h>

// Defines the fields hidden from callers of the public API.
struct DynamicArray {
    // Counts the item-pointer slots currently allocated.
    size_t capacity;
    // Counts the item pointers currently stored.
    size_t size;
    // Points to the dynamically allocated contiguous item buffer.
    void **items;
};

// Allocates and initializes an empty dynamic array.
DynamicArray *dynamic_array_create(void) {
    // Allocates memory for the DynamicArray structure.
    DynamicArray *array = malloc(sizeof(DynamicArray));
    // Checks whether allocation failed.
    if (array == NULL) {
        // Reports allocation failure to the caller.
        return NULL;
    }

    // Initializes the logical item count as zero.
    array->size = 0U;
    // Initializes capacity because no item slots are allocated yet.
    array->capacity = 0U;
    // Initializes the item-buffer pointer as a null pointer.
    array->items = NULL;
    // Returns the initialized dynamic array.
    return array;
}

// Frees the allocations owned by the dynamic array.
void dynamic_array_destroy(DynamicArray *array) {
    // Makes destruction of a null dynamic array a safe no-op.
    if (array == NULL) {
        // Returns because there is no allocation to release.
        return;
    }

    // Frees the pointer buffer but not its caller-owned item values.
    free(array->items);
    // Frees the DynamicArray structure.
    free(array);
    // Ends the void function.
    return;
}

// Copies an item pointer at an index without modifying the array.
bool dynamic_array_get(const DynamicArray *array, size_t index, void **out_item) {
    // Rejects a missing dynamic-array pointer.
    if (array == NULL) {
        // Reports failure because there is no array to inspect.
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        // Reports failure without changing an output pointer.
        return false;
    }

    // Rejects indexes outside the valid item range.
    if (index >= array->size) {
        // Leaves the caller's output pointer unchanged.
        return false;
    }

    // Copies the requested caller-owned item pointer to the output location.
    *out_item = array->items[index];
    // Reports successful lookup without changing the array.
    return true;
}

// Replaces an item pointer at an index and returns the replaced pointer.
bool dynamic_array_set(DynamicArray *array, size_t index, void *item, void **out_old_item) {
    // Rejects a missing dynamic-array pointer.
    if (array == NULL) {
        // Reports failure because there is no array to modify.
        return false;
    }

    // Rejects a missing output location.
    if (out_old_item == NULL) {
        // Reports failure without replacing an item.
        return false;
    }

    // Rejects indexes outside the valid item range.
    if (index >= array->size) {
        // Leaves the caller's output pointer and array unchanged.
        return false;
    }

    // Copies the replaced caller-owned item pointer to the output location.
    *out_old_item = array->items[index];
    // Stores the new caller-owned item pointer at the requested index.
    array->items[index] = item;
    // Reports successful replacement.
    return true;
}

// Inserts an item pointer before the item currently at index.
bool dynamic_array_insert(DynamicArray *array, size_t index, void *item) {
    // Rejects a missing dynamic-array pointer.
    if (array == NULL) {
        // Reports failure because there is no array to modify.
        return false;
    }

    // Rejects indexes beyond the valid append position.
    if (index > array->size) {
        // Leaves the array unchanged and reports failure.
        return false;
    }

    // Grows the buffer only when every allocated slot is occupied.
    if (array->size == array->capacity) {
        // Allocates two slots initially and doubles capacity afterward.
        size_t new_capacity = array->capacity == 0U ? 2U : array->capacity * 2U;
        // Requests enough bytes for the new number of pointer slots.
        void **items = realloc(array->items, new_capacity * sizeof(*array->items));
        // Checks whether resizing failed.
        if (items == NULL) {
            // Preserves the old buffer and reports failure.
            return false;
        }

        // Stores the possibly relocated item-buffer address.
        array->items = items;
        // Records the new number of available slots.
        array->capacity = new_capacity;
    }

    // Starts at the first unused slot to make room for the new item.
    for (size_t i = array->size; i > index; i--) {
        // Moves each preceding item pointer one slot to the right.
        array->items[i] = array->items[i - 1U];
    }

    // Stores the new caller-owned item pointer at the insertion index.
    array->items[index] = item;
    // Increases the logical item count.
    array->size++;
    // Reports successful insertion.
    return true;
}

// Removes an item pointer at an index and returns it through out_item.
bool dynamic_array_remove(DynamicArray *array, size_t index, void **out_item) {
    // Rejects a missing dynamic-array pointer.
    if (array == NULL) {
        // Reports failure because there is no array to modify.
        return false;
    }

    // Rejects indexes outside the valid item range.
    if (index >= array->size) {
        // Leaves the array and caller's output pointer unchanged.
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        // Reports failure without removing an item.
        return false;
    }

    // Copies the removed caller-owned item pointer to the output location.
    *out_item = array->items[index];

    // Shifts later item pointers left to close the removed slot.
    for (size_t i = index; i < array->size - 1U; i++) {
        // Copies the next pointer into the current slot.
        array->items[i] = array->items[i + 1U];
    }

    // Clears the now-unused final slot.
    array->items[array->size - 1U] = NULL;
    // Decreases the logical item count.
    array->size--;
    // Reports successful removal.
    return true;
}

// Returns the number of item pointers stored.
size_t dynamic_array_size(const DynamicArray *array) {
    // Treats a null dynamic array as containing zero items.
    if (array == NULL) {
        // Returns the empty-array count.
        return 0U;
    }

    // Returns the current logical item count.
    return array->size;
}

// Returns the number of item-pointer slots allocated.
size_t dynamic_array_capacity(const DynamicArray *array) {
    // Treats a null dynamic array as having zero slots.
    if (array == NULL) {
        // Returns the no-capacity count.
        return 0U;
    }

    // Returns the current allocated slot count.
    return array->capacity;
}

// Reports whether a dynamic array contains no item pointers.
bool dynamic_array_is_empty(const DynamicArray *array) {
    // Treats a null dynamic array as empty under this API contract.
    if (array == NULL) {
        // Reports an empty dynamic array.
        return true;
    }

    // Compares the logical item count with zero.
    return array->size == 0U;
}
