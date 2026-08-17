#include <stdbool.h>
#include <stddef.h>

typedef struct DynamicArray DynamicArray;

DynamicArray *dynamic_array_create(void);
void dynamic_array_destroy(DynamicArray *array);
bool dynamic_array_get(const DynamicArray *array, size_t index, void **out_item);
bool dynamic_array_set(DynamicArray *array, size_t index, void *item, void **out_old_item);
bool dynamic_array_insert(DynamicArray *array, size_t index, void *item);
bool dynamic_array_remove(DynamicArray *array, size_t index, void **out_item);
size_t dynamic_array_size(const DynamicArray *array);
size_t dynamic_array_capacity(const DynamicArray *array);
bool dynamic_array_is_empty(const DynamicArray *array);
