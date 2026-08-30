// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>
// Declares uint32_t.
#include <stdint.h>

// Stably sorts keys in the declared range [0, key_limit).
bool counting_sort(uint32_t *items, size_t count, uint32_t key_limit);
