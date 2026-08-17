// Provides the reusable benchmark API.
#include "benchmark.h"
// Provides the public singly linked-list API.
#include "../src/data-structures/linear/linked/singly-linked-list/singly_linked_list.h"

// Uses a smaller default because several linked-list operations are O(n).
#ifndef BENCHMARK_ITEM_COUNT
#define BENCHMARK_ITEM_COUNT 2000U
#endif

// Defines the number of operations in each timed sample.
enum { ITEM_COUNT = BENCHMARK_ITEM_COUNT };
// Defines enough stable values for middle insertion to double the list size.
enum { VALUE_COUNT = ITEM_COUNT * 2U };

// Stores state shared by singly linked-list benchmarks.
struct ListContext {
    // Owns the list created before each sample.
    SinglyLinkedList *list;
    // Stores caller-owned values used by list operations.
    int values[VALUE_COUNT];
    // Tracks the number of nodes currently linked into the list.
    size_t count;
    // Tracks the next unused caller-owned value.
    size_t next_value;
    // Receives pointers returned by list operations.
    void *out_item;
};

// Creates an empty list for front or back insertion measurement.
static bool empty_setup(void *context) {
    struct ListContext *list_context = context;

    list_context->list = singly_linked_list_create();
    list_context->count = 0U;
    list_context->next_value = 0U;
    return list_context->list != NULL;
}

// Creates a list with ITEM_COUNT nodes outside the timed region.
static bool populated_setup(void *context) {
    struct ListContext *list_context = context;

    list_context->list = singly_linked_list_create();
    if (list_context->list == NULL) {
        return false;
    }

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        if (!singly_linked_list_push_back(list_context->list, &list_context->values[index])) {
            singly_linked_list_destroy(list_context->list);
            list_context->list = NULL;
            return false;
        }
    }

    list_context->count = ITEM_COUNT;
    list_context->next_value = ITEM_COUNT;
    return true;
}

// Inserts one value at the front of an initially empty list.
static bool push_front_operation(void *context) {
    struct ListContext *list_context = context;

    if (list_context->next_value == ITEM_COUNT) {
        return false;
    }

    if (!singly_linked_list_push_front(
            list_context->list,
            &list_context->values[list_context->next_value]
        )) {
        return false;
    }

    list_context->count++;
    list_context->next_value++;
    return true;
}

// Inserts one value at the back of an initially empty list.
static bool push_back_operation(void *context) {
    struct ListContext *list_context = context;

    if (list_context->next_value == ITEM_COUNT) {
        return false;
    }

    if (!singly_linked_list_push_back(
            list_context->list,
            &list_context->values[list_context->next_value]
        )) {
        return false;
    }

    list_context->count++;
    list_context->next_value++;
    return true;
}

// Removes one node from the front of a populated list.
static bool pop_front_operation(void *context) {
    struct ListContext *list_context = context;

    if (list_context->count == 0U) {
        return false;
    }

    if (!singly_linked_list_pop_front(list_context->list, &list_context->out_item)) {
        return false;
    }

    list_context->count--;
    return true;
}

// Removes one node from the back of a populated list.
static bool pop_back_operation(void *context) {
    struct ListContext *list_context = context;

    if (list_context->count == 0U) {
        return false;
    }

    if (!singly_linked_list_pop_back(list_context->list, &list_context->out_item)) {
        return false;
    }

    list_context->count--;
    return true;
}

// Looks up the current middle node without changing list state.
static bool get_middle_operation(void *context) {
    struct ListContext *list_context = context;

    return singly_linked_list_get(
        list_context->list,
        list_context->count / 2U,
        &list_context->out_item
    );
}

// Inserts one value at the current middle index.
static bool insert_middle_operation(void *context) {
    struct ListContext *list_context = context;

    if (list_context->next_value == VALUE_COUNT) {
        return false;
    }

    if (!singly_linked_list_insert(
            list_context->list,
            list_context->count / 2U,
            &list_context->values[list_context->next_value]
        )) {
        return false;
    }

    list_context->count++;
    list_context->next_value++;
    return true;
}

// Removes one node at the current middle index.
static bool remove_middle_operation(void *context) {
    struct ListContext *list_context = context;

    if (list_context->count == 0U) {
        return false;
    }

    if (!singly_linked_list_remove(
            list_context->list,
            list_context->count / 2U,
            &list_context->out_item
        )) {
        return false;
    }

    list_context->count--;
    return true;
}

// Confirms the list's public size matches tracked state.
static bool size_verify(void *context) {
    struct ListContext *list_context = context;

    return singly_linked_list_size(list_context->list) == list_context->count;
}

// Releases the list created for one benchmark batch.
static void list_teardown(void *context) {
    struct ListContext *list_context = context;

    singly_linked_list_destroy(list_context->list);
    list_context->list = NULL;
}

// Runs and prints one configured linked-list benchmark.
static bool run_benchmark(const BenchmarkConfig *config) {
    BenchmarkResult result;

    if (!benchmark_run(config, &result)) {
        return false;
    }

    benchmark_print_result(&result);
    return true;
}

int main(void) {
    struct ListContext list_context = { NULL, { 0 }, 0U, 0U, NULL };
    BenchmarkConfig config = {
        .name = "Singly linked list push front",
        .warmup_iterations = 200U,
        .sample_count = 21U,
        .iterations_per_sample = ITEM_COUNT,
        .setup = empty_setup,
        .operation = push_front_operation,
        .verify = size_verify,
        .teardown = list_teardown,
        .context = &list_context
    };

    for (size_t index = 0U; index < VALUE_COUNT; index++) {
        list_context.values[index] = (int)index;
    }

    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Singly linked list push back";
    config.operation = push_back_operation;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Singly linked list pop front";
    config.setup = populated_setup;
    config.operation = pop_front_operation;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Singly linked list pop back";
    config.operation = pop_back_operation;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Singly linked list get middle";
    config.operation = get_middle_operation;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Singly linked list insert middle";
    config.operation = insert_middle_operation;
    if (!run_benchmark(&config)) {
        return 1;
    }

    config.name = "Singly linked list remove middle";
    config.operation = remove_middle_operation;
    if (!run_benchmark(&config)) {
        return 1;
    }

    return 0;
}
