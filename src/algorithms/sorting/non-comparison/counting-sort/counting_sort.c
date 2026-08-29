#include "counting_sort.h"
// Declares memcpy.
#include <string.h>
// Declares malloc and free.
#include <stdlib.h>

// Stably sorts unsigned integer keys in the declared range [0, key_limit).
bool counting_sort(uint32_t *items, size_t count, uint32_t key_limit) {
    // Treats an empty range as already sorted, even without array storage.
    if (count == 0U) {
        return true;
    }

    // Rejects a missing nonempty input array.
    if (items == NULL) {
        return false;
    }

    // Rejects a range with no valid key values.
    if (key_limit == 0U) {
        return false;
    }

    // Allocates one count slot for every valid key value.
    size_t *counts = malloc(sizeof(size_t) * key_limit);
    // Reports counts allocation failure before changing input items.
    if (counts == NULL) {
        return false;
    }

    // Initializes every key count to zero.
    size_t n = 0U;
    while (n < key_limit) {
        counts[n] = 0U;
        n++;
    }

    // Counts each input key after confirming it belongs to the declared range.
    n = 0U;
    while(n < count) {
        if (items[n] < key_limit) {
            counts[items[n]]++;
            n++;
        } else {
            free(counts);
            return false;
        }
    }

    // Converts raw key counts into exclusive ending positions by prefix sum.
    n = 0U;
    size_t k = 0U;
    while(n < key_limit) {
        k = k + counts[n];
        counts[n] = k;
        n++;
    }

    // Rejects an output-array byte size that would overflow.
    if (count > SIZE_MAX / sizeof(uint32_t)) {
        free(counts);
        return false;
    }

    // Allocates the stable sorted-output array.
    uint32_t *out_items = malloc(sizeof(uint32_t) * count);
    // Reports output allocation failure without changing input items.
    if (out_items == NULL) {
        free(counts);
        return false;
    }

    // Places original items in reverse input order to preserve equal-key order.
    size_t i = count - 1;
    while(i >= 0U) {
        // Decrements to the next open slot in this key's output block.
        counts[items[i]]--;
        // Copies this key into its final stable output position.
        out_items[counts[items[i]]] = items[i];
        // Stops after processing the first input item without unsigned wrap.
        if (i == 0U) {
            break;
        } else {
            i--;
        }
    }

    // Copies every sorted output key back into the caller-owned input array.
    memcpy(items, out_items, sizeof(uint32_t) * count);

    // Releases both auxiliary allocations after copy-back completes.
    free(counts);
    free(out_items);

    // Reports successful stable in-place-visible sorting.
    return true;
}
