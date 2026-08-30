// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Names the opaque growable pointer array.
typedef struct DynamicArray DynamicArray;

// Allocates an empty dynamic array.
DynamicArray *dynamic_array_create(void);
// Frees array storage without freeing caller-owned items.
void dynamic_array_destroy(DynamicArray *array);
// Returns one stored item by index.
bool dynamic_array_get(const DynamicArray *array, size_t index, void **out_item);
// Replaces one stored item and optionally returns the previous item.
bool dynamic_array_set(DynamicArray *array, size_t index, void *item, void **out_old_item);
// Inserts one item at index, shifting later items right.
bool dynamic_array_insert(DynamicArray *array, size_t index, void *item);
// Removes one item at index, shifting later items left.
bool dynamic_array_remove(DynamicArray *array, size_t index, void **out_item);
// Returns the number of stored items.
size_t dynamic_array_size(const DynamicArray *array);
// Returns allocated item-slot capacity.
size_t dynamic_array_capacity(const DynamicArray *array);
// Reports whether the array contains no items.
bool dynamic_array_is_empty(const DynamicArray *array);
