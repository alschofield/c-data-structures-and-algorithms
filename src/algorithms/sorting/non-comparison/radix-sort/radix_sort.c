// Declares the public radix-sort API.
#include "radix_sort.h"
// Declares malloc and free.
#include <stdlib.h>
// Declares memcpy.
#include <string.h>

// Stably counting-sorts full integer values by one selected byte.
static bool radix_count(uint32_t *items, size_t count, size_t shift) {
    // Allocates one count slot for every possible byte value.
    size_t *counts = malloc(sizeof(size_t) * 256);
    // Reports counts allocation failure before changing input items.
    if (counts == NULL) {
        return false;
    }

    // Initializes every byte-key count to zero.
    size_t n = 0U;
    while(n < 256) {
        counts[n] = 0U;
        n++;
    }

    // Counts the selected byte from every full integer value.
    n = 0U;
    while(n < count) {
        counts[(items[n] >> shift) & 0xFFU]++;
        n++;
    }

    // Converts raw byte counts into exclusive ending positions by prefix sum.
    size_t k = 0U;
    n = 0U;
    while(n < 256) {
        k = counts[n] + k;
        counts[n] = k;
        n++;
    }

    // Rejects an output-array byte size that would overflow.
    if (count > SIZE_MAX / sizeof(uint32_t)) {
        free(counts);
        return false;
    }

    // Allocates the stable output array for this byte pass.
    uint32_t *out_items = malloc(sizeof(uint32_t) * count);
    // Reports output allocation failure without changing input items.
    if (out_items == NULL) {
        free(counts);
        return false;
    }

    // Places full values in reverse input order to preserve equal-byte order.
    n = count - 1;
    while(n >= 0) {
        // Decrements to the next open slot in this key's output block.
        counts[(items[n] >> shift) & 0xFFU]--;
        // Copies this key into its final stable output position.
        out_items[counts[(items[n] >> shift) & 0xFFU]] = items[n];
        // Stops after processing the first input item without unsigned wrap.
        if (n == 0U) {
            break;
        } else {
            n--;
        }
    }

    // Makes this stable byte ordering the input order for the next byte pass.
    memcpy(items, out_items, sizeof(uint32_t) * count);

    // Releases both auxiliary allocations after copy-back completes.
    free(counts);
    free(out_items);

    // Reports successful stable byte sorting.
    return true;
}

// Sorts full 32-bit unsigned integers by four least-to-most-significant bytes.
bool radix_sort(uint32_t *items, size_t count) {
    // Treats an empty range as sorted, even without array storage.
    if (count == 0U) {
        return true;
    }

    // Rejects a missing nonempty input array.
    if (items == NULL) {
        return false;
    }

    // Treats one value as already sorted.
    if (count == 1U) {
        return true;
    }

    // Stably sorts by bytes from least to most significant.
    if (!radix_count(items, count, 0U)) {
        return false;
    }

    if (!radix_count(items, count, 8U)) {
        return false;
    }

    if (!radix_count(items, count, 16U)) {
        return false;
    }

    if (!radix_count(items, count, 24U)) {
        return false;
    }

    // Reports successful full-width stable radix sorting.
    return true;
}
