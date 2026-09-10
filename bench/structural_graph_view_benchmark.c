// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the index-based graph algorithms under comparison.
#include "../src/algorithms/graph-traversal/breadth-first-search/breadth_first_search.h"
#include "../src/algorithms/graph-traversal/depth-first-search/depth_first_search.h"
#include "../src/algorithms/shortest-paths/dijkstra/dijkstra.h"
#include "../src/algorithms/shortest-paths/a-star/a_star.h"
// Provides structural GraphView adapters.
#include "../src/data-structures/linear/linked/singly-linked-list/singly_linked_list.h"
#include "../src/data-structures/linear/linked/doubly-linked-list/doubly_linked_list.h"
#include "../src/data-structures/trees/heaps/binary-heap/binary_heap.h"
#include "../src/data-structures/trees/binary-search-trees/binary-search-tree/binary_search_tree.h"
#include "../src/data-structures/trees/tries/prefix-trie/prefix_trie.h"
// Declares snprintf.
#include <stdio.h>

// Uses a compact baseline unless Make supplies a larger comparison size.
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 64U
#endif

enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };
// Reduces large-workload sampling while retaining a resistant median.
enum { SAMPLE_COUNT = ITEM_COUNT >= 1024U ? 5U : 21U };

// Holds common graph algorithm state for every structural workload.
struct StructuralContext {
    GraphView view;
    void *structure;
    void (*destroy)(void *structure);
    size_t count;
    size_t source;
    size_t goal;
    uint64_t expected_goal_distance;
    size_t visited;
    uint64_t distances[ITEM_COUNT];
    size_t parents[ITEM_COUNT];
    size_t path[ITEM_COUNT];
    size_t path_length;
};

struct SinglyContext { struct StructuralContext base; int values[ITEM_COUNT]; };
struct DoublyContext { struct StructuralContext base; int values[ITEM_COUNT]; };
struct HeapContext { struct StructuralContext base; int values[ITEM_COUNT]; };
struct BstContext { struct StructuralContext base; int values[ITEM_COUNT]; };
struct TrieContext { struct StructuralContext base; char key[ITEM_COUNT]; };

static void destroy_singly(void *structure) { singly_linked_list_destroy(structure); }
static void destroy_doubly(void *structure) { doubly_linked_list_destroy(structure); }
static void destroy_heap(void *structure) { binary_heap_destroy(structure); }
static void destroy_bst(void *structure) { binary_search_tree_destroy(structure); }
static void destroy_trie(void *structure) { prefix_trie_destroy(structure); }

static int compare_ints(const void *left, const void *right) {
    const int left_value = *(const int *)left;
    const int right_value = *(const int *)right;
    return (left_value > right_value) - (left_value < right_value);
}

static uint64_t zero_heuristic(size_t index, void *context) {
    (void)index;
    (void)context;
    return 0U;
}

static bool count_visit(size_t index, void *context) {
    (void)index;
    ((struct StructuralContext *)context)->visited++;
    return true;
}

static void reset_state(struct StructuralContext *context) {
    context->visited = 0U;
    context->path_length = 0U;
}

static bool setup_singly(void *context) {
    struct SinglyContext *state = context;
    SinglyLinkedList *list = singly_linked_list_create();
    if (list == NULL) return false;
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        state->values[index] = (int)index;
        if (!singly_linked_list_push_back(list, &state->values[index])) {
            singly_linked_list_destroy(list);
            return false;
        }
    }
    if (!singly_linked_list_graph_view(list, &state->base.view)) {
        singly_linked_list_destroy(list);
        return false;
    }
    state->base.structure = list; state->base.destroy = destroy_singly;
    state->base.count = ITEM_COUNT; state->base.source = 0U;
    state->base.goal = ITEM_COUNT - 1U; state->base.expected_goal_distance = ITEM_COUNT - 1U;
    reset_state(&state->base);
    return true;
}

static bool setup_doubly(void *context) {
    struct DoublyContext *state = context;
    DoublyLinkedList *list = doubly_linked_list_create();
    if (list == NULL) return false;
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        state->values[index] = (int)index;
        if (!doubly_linked_list_push_back(list, &state->values[index])) {
            doubly_linked_list_destroy(list);
            return false;
        }
    }
    if (!doubly_linked_list_graph_view(list, &state->base.view)) {
        doubly_linked_list_destroy(list);
        return false;
    }
    state->base.structure = list; state->base.destroy = destroy_doubly;
    state->base.count = ITEM_COUNT; state->base.source = 0U;
    state->base.goal = ITEM_COUNT - 1U; state->base.expected_goal_distance = ITEM_COUNT - 1U;
    reset_state(&state->base);
    return true;
}

static bool setup_heap(void *context) {
    struct HeapContext *state = context;
    BinaryHeap *heap = binary_heap_create(compare_ints);
    if (heap == NULL) return false;
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        state->values[index] = (int)(ITEM_COUNT - index);
        if (!binary_heap_push(heap, &state->values[index])) {
            binary_heap_destroy(heap);
            return false;
        }
    }
    if (!binary_heap_graph_view(heap, &state->base.view)) {
        binary_heap_destroy(heap);
        return false;
    }
    size_t goal = ITEM_COUNT - 1U;
    uint64_t depth = 0U;
    while (goal > 0U) { goal = (goal - 1U) / 2U; depth++; }
    state->base.structure = heap; state->base.destroy = destroy_heap;
    state->base.count = ITEM_COUNT; state->base.source = 0U;
    state->base.goal = ITEM_COUNT - 1U; state->base.expected_goal_distance = depth;
    reset_state(&state->base);
    return true;
}

static bool insert_balanced(BinarySearchTree *tree, int *values, size_t low,
                            size_t high) {
    if (low >= high) return true;
    size_t middle = low + (high - low) / 2U;
    return binary_search_tree_insert(tree, &values[middle]) &&
        insert_balanced(tree, values, low, middle) &&
        insert_balanced(tree, values, middle + 1U, high);
}

// Returns the root-to-smallest-item distance in this median-insertion BST shape.
static uint64_t bst_goal_distance(size_t count) {
    size_t low = 0U;
    size_t high = count;
    uint64_t distance = 0U;

    while (low < high) {
        size_t middle = low + (high - low) / 2U;
        if (middle == 0U) return distance;
        high = middle;
        distance++;
    }
    return distance;
}

static bool setup_bst(void *context) {
    struct BstContext *state = context;
    BinarySearchTree *tree = binary_search_tree_create(compare_ints);
    if (tree == NULL) return false;
    for (size_t index = 0U; index < ITEM_COUNT; index++) state->values[index] = (int)index;
    if (!insert_balanced(tree, state->values, 0U, ITEM_COUNT) ||
        !binary_search_tree_graph_view(tree, &state->base.view)) {
        binary_search_tree_destroy(tree);
        return false;
    }
    state->base.structure = tree; state->base.destroy = destroy_bst;
    state->base.count = ITEM_COUNT; state->base.source = ITEM_COUNT / 2U;
    state->base.goal = 0U; state->base.expected_goal_distance = bst_goal_distance(ITEM_COUNT);
    reset_state(&state->base);
    return true;
}

static bool setup_trie(void *context) {
    struct TrieContext *state = context;
    PrefixTrie *trie = prefix_trie_create();
    if (trie == NULL) return false;
    for (size_t index = 0U; index + 1U < ITEM_COUNT; index++) state->key[index] = 'a';
    state->key[ITEM_COUNT - 1U] = '\0';
    if (!prefix_trie_insert(trie, state->key) ||
        !prefix_trie_graph_view(trie, &state->base.view)) {
        prefix_trie_destroy(trie);
        return false;
    }
    state->base.structure = trie; state->base.destroy = destroy_trie;
    state->base.count = ITEM_COUNT; state->base.source = 0U;
    state->base.goal = ITEM_COUNT - 1U; state->base.expected_goal_distance = ITEM_COUNT - 1U;
    reset_state(&state->base);
    return true;
}

static void teardown(void *context) {
    struct StructuralContext *state = context;
    state->destroy(state->structure);
    state->structure = NULL;
}

static bool bfs_operation(void *context) {
    struct StructuralContext *state = context;
    return breadth_first_search(&state->view, state->source, count_visit, state);
}
static bool dfs_operation(void *context) {
    struct StructuralContext *state = context;
    return depth_first_search(&state->view, state->source, count_visit, state);
}
static bool dijkstra_operation(void *context) {
    struct StructuralContext *state = context;
    return dijkstra(&state->view, state->source, state->distances, state->parents);
}
static bool a_star_operation(void *context) {
    struct StructuralContext *state = context;
    return a_star(&state->view, state->source, state->goal, zero_heuristic, NULL,
                  state->path, ITEM_COUNT, &state->path_length);
}
static bool traversal_verify(void *context) {
    struct StructuralContext *state = context;
    return state->visited == state->count;
}
static bool dijkstra_verify(void *context) {
    struct StructuralContext *state = context;
    return state->distances[state->goal] == state->expected_goal_distance;
}
static bool a_star_verify(void *context) {
    struct StructuralContext *state = context;
    return state->path_length == state->expected_goal_distance + 1U &&
        state->path[0] == state->source &&
        state->path[state->path_length - 1U] == state->goal;
}
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;
    if (!benchmark_run(config, &result)) return false;
    benchmark_print_result(&result);
    return true;
}

static bool run_structure(const char *label, void *context,
                          BenchmarkCallback setup) {
    char name[96];
    BenchmarkConfig config = {
        .name = name, .warmup_iterations = 1U, .sample_count = SAMPLE_COUNT,
        .iterations_per_sample = 1U, .setup = setup, .operation = bfs_operation,
        .verify = traversal_verify, .teardown = teardown, .context = context
    };
    snprintf(name, sizeof(name), "%s BFS", label);
    if (!run_benchmark(&config)) return false;
    snprintf(name, sizeof(name), "%s DFS", label); config.operation = dfs_operation;
    if (!run_benchmark(&config)) return false;
    snprintf(name, sizeof(name), "%s Dijkstra", label); config.operation = dijkstra_operation; config.verify = dijkstra_verify;
    if (!run_benchmark(&config)) return false;
    snprintf(name, sizeof(name), "%s A-star", label); config.operation = a_star_operation; config.verify = a_star_verify;
    return run_benchmark(&config);
}

int main(void) {
    static struct SinglyContext singly;
    static struct DoublyContext doubly;
    static struct HeapContext heap;
    static struct BstContext bst;
    static struct TrieContext trie;
    if (!run_structure("Singly linked list", &singly, setup_singly)) return 1;
    if (!run_structure("Doubly linked list", &doubly, setup_doubly)) return 1;
    if (!run_structure("Binary heap", &heap, setup_heap)) return 1;
    if (!run_structure("Binary search tree", &bst, setup_bst)) return 1;
    return run_structure("Prefix trie", &trie, setup_trie) ? 0 : 1;
}
