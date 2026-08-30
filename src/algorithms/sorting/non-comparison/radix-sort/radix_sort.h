// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>
// Declares uint32_t.
#include <stdint.h>

// Stably sorts full-width unsigned integer keys by four byte passes.
bool radix_sort(uint32_t *items, size_t count);
