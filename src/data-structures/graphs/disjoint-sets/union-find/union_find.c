// Declares the public union-find API.
#include "union_find.h"
// Declares malloc and free.
#include <stdlib.h>
// Declares SIZE_MAX.
#include <stdint.h>

// Defines the arrays and counts hidden from callers.
struct UnionFind {
    // Stores the fixed number of valid dense elements.
    size_t capacity;
    // Stores the current number of disconnected sets.
    size_t groups;
    // Stores each element's immediate parent index.
    size_t *parents;
    // Stores tree-rank estimates for root-link decisions.
    size_t *ranks;
};

// Allocates one singleton disjoint set per dense element.
UnionFind *union_find_create(size_t element_count) {
    // Allocates the union-find wrapper before its arrays.
    UnionFind *set = malloc(sizeof(UnionFind));
    // Reports wrapper allocation failure.
    if (set == NULL) {
        return NULL;
    }

    // Rejects a parent-array byte size that would overflow.
    if (element_count > SIZE_MAX / sizeof(size_t)) {
        free(set);
        return NULL;
    }

    // Allocates the contiguous immediate-parent index array.
    size_t *parents = malloc(sizeof(size_t) * element_count);
    // Releases the wrapper when parent allocation fails.
    if (parents == NULL) {
        free(set);
        return NULL;
    }

    // Rejects a rank-array byte size that would overflow.
    if (element_count > SIZE_MAX / sizeof(size_t)) {
        free(set);
        free(parents);
        return NULL;
    }

    // Allocates the contiguous root-rank array.
    size_t *ranks = malloc(sizeof(size_t) * element_count);
    // Releases prior allocations when rank allocation fails.
    if (ranks == NULL) {
        free(set);
        free(parents);
        return NULL;
    }

    // Initializes every element as a rank-zero singleton root.
    for(size_t n = 0U; n < element_count; n++) {
        parents[n] = n;
        ranks[n] = 0U;   
    }

    // Records valid element bounds and the initial singleton group count.
    set->capacity = element_count;
    set->groups = element_count;
    set->parents = parents;
    set->ranks = ranks;

    // Returns the initialized disjoint-set forest.
    return set;
}

// Releases both owned arrays and the union-find wrapper.
void union_find_destroy(UnionFind *set) {
    // Treats a missing union-find as a no-op.
    if (set == NULL) {
        return;
    }

    // Releases the parent and rank arrays before the wrapper.
    free(set->parents);
    free(set->ranks);
    free(set);

    // Ends the no-op destruction operation.
    return;
}

// Returns the root representative for one valid dense element.
bool union_find_find(UnionFind *set, size_t element, size_t *out_representative) {
    // Rejects a missing union-find.
    if (set == NULL) {
        return false;
    }

    // Rejects an element outside the fixed dense range.
    if (element >= set->capacity) {
        return false;
    }

    // Rejects a missing caller-owned representative output location.
    if (out_representative == NULL) {
        return false;
    }

    // Walks immediate parent links until reaching a self-parented root.
    size_t index = element;
    size_t root = element;
    while(index != set->parents[index]) {
        index = set->parents[index];
        root = set->parents[index];
    }

    // Returns the root representative index.
    *out_representative = set->parents[index];

    // Walks the original chain again and repoints every visited element at the root.
    index = element;
    while (index != set->parents[index]) {
        // Preserves the current element before following its original parent link.
        size_t prev = index;
        // Advances before the saved element's parent is overwritten.
        index = set->parents[index];
        // Flattens this segment of the tree for future representative lookups.
        set->parents[prev] = root;
    }

    // Reports successful representative lookup and path compression.
    return true;
}

// Merges two disjoint root sets by rank and reports whether they changed.
bool union_find_union(UnionFind *set, size_t a, size_t b, bool *out_merged) {
    // Rejects a missing union-find.
    if (set == NULL) {
        return false;
    }

    // Rejects an out-of-range first element.
    if (a >= set->capacity) {
        return false;
    }

    // Rejects an out-of-range second element.
    if (b >= set->capacity) {
        return false;
    }

    // Rejects a missing caller-owned merge-result location.
    if (out_merged == NULL) {
        return false;
    }

    // Holds each input set's root representative after path compression.
    size_t a_root = a;
    size_t b_root = b;

    // Resolves and compresses the first element's parent chain.
    if (!union_find_find(set, a, &a_root)) {
        return false;
    }

    // Resolves and compresses the second element's parent chain.
    if (!union_find_find(set, b, &b_root)){
        return false;
    }

    // Reports a redundant union without changing rank or group count.
    if (a_root == b_root) {
        *out_merged = false;
        return true;
    }

    // Attaches the lower-rank root beneath the higher-rank root.
    if (set->ranks[a_root] > set->ranks[b_root]){
        set->parents[b_root] = a_root;
    } else if (set->ranks[a_root] < set->ranks[b_root]) {
        set->parents[a_root] = b_root;
    } else {
        // Uses the first root as a deterministic tie winner.
        set->parents[b_root] = a_root;
        // Increases rank because equal-height trees became one level taller.
        set->ranks[a_root]++;
    }

    // Records that two previously separate sets became one.
    set->groups--;
    *out_merged = true;

    // Reports successful merge processing.
    return true;
}

// Reports whether two valid elements reach the same root representative.
bool union_find_connected(UnionFind *set, size_t a, size_t b, bool *out_connected) {
    // Rejects a missing union-find.
    if (set == NULL) {
        return false;
    }

    // Rejects an out-of-range first element.
    if (a >= set->capacity) {
        return false;
    }

    // Rejects an out-of-range second element.
    if (b >= set->capacity) {
        return false;
    }

    // Rejects a missing caller-owned connectivity output location.
    if (out_connected == NULL) {
        return false;
    }

    size_t out_a = 0U;
    size_t out_b = 0U;

    if (!union_find_find(set, a, &out_a)) {
        return false;
    }

    if (!union_find_find(set, b, &out_b)) {
        return false;
    }

    // Compares roots to determine whether both elements share one set.
    *out_connected = out_a == out_b;

    // Reports that the connectivity query completed successfully.
    return true;
}

// Reports the number of currently disconnected sets.
size_t union_find_set_count(const UnionFind *set) {
    // Treats a missing union-find as containing zero sets.
    if (set == NULL) {
        return false;
    }

    // Returns the count changed by each effective union.
    return set->groups;
}
