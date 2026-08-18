#include "binary_heap.h"

#include <assert.h>
#include <stddef.h>

static int compare_ints(const void *left, const void *right) {
    const int *left_value = left;
    const int *right_value = right;

    if (*left_value < *right_value) {
        return -1;
    }
    if (*left_value > *right_value) {
        return 1;
    }
    return 0;
}

static void test_empty_heap(void) {
    BinaryHeap *heap = binary_heap_create(compare_ints);
    int sentinel = 0;
    void *out = &sentinel;

    assert(heap != NULL);
    assert(binary_heap_size(heap) == 0U);
    assert(binary_heap_is_empty(heap));
    assert(!binary_heap_pop(heap, &out));
    assert(out == &sentinel);
    assert(!binary_heap_peek(heap, &out));
    assert(out == &sentinel);
    assert(!binary_heap_pop(heap, NULL));
    assert(!binary_heap_peek(heap, NULL));
    binary_heap_destroy(heap);
}

static void test_orders_by_comparison(void) {
    BinaryHeap *heap = binary_heap_create(compare_ints);
    int values[] = { 7, 3, 9, 1, 5 };
    int expected[] = { 1, 3, 5, 7, 9 };
    void *out = NULL;

    assert(heap != NULL);
    for (size_t index = 0U; index < 5U; index++) {
        assert(binary_heap_push(heap, &values[index]));
    }
    assert(binary_heap_size(heap) == 5U);

    assert(binary_heap_peek(heap, &out));
    assert(*(int *)out == 1);
    assert(binary_heap_size(heap) == 5U);

    for (size_t index = 0U; index < 5U; index++) {
        assert(binary_heap_pop(heap, &out));
        assert(*(int *)out == expected[index]);
    }
    assert(binary_heap_is_empty(heap));
    binary_heap_destroy(heap);
}

static void test_interleaved_push_pop(void) {
    BinaryHeap *heap = binary_heap_create(compare_ints);
    int a = 4;
    int b = 2;
    int c = 6;
    int d = 1;
    void *out = NULL;

    assert(heap != NULL);
    assert(binary_heap_push(heap, &a));
    assert(binary_heap_push(heap, &b));
    assert(binary_heap_pop(heap, &out));
    assert(out == &b);
    assert(binary_heap_push(heap, &c));
    assert(binary_heap_push(heap, &d));
    assert(binary_heap_pop(heap, &out));
    assert(out == &d);
    assert(binary_heap_pop(heap, &out));
    assert(out == &a);
    assert(binary_heap_pop(heap, &out));
    assert(out == &c);
    assert(binary_heap_is_empty(heap));
    binary_heap_destroy(heap);
}

static void test_growth_and_reuse(void) {
    enum { ITEM_COUNT = 1024 };
    BinaryHeap *heap = binary_heap_create(compare_ints);
    static int values[ITEM_COUNT];
    void *out = NULL;
    int previous = -1;
    unsigned int seed = 12345U;

    assert(heap != NULL);
    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        seed = seed * 1103515245U + 12345U;
        values[index] = (int)(seed % 100000U);
        assert(binary_heap_push(heap, &values[index]));
    }
    assert(binary_heap_size(heap) == (size_t)ITEM_COUNT);

    for (size_t index = 0U; index < ITEM_COUNT; index++) {
        assert(binary_heap_pop(heap, &out));
        assert(*(int *)out >= previous);
        previous = *(int *)out;
    }
    assert(binary_heap_is_empty(heap));
    assert(binary_heap_push(heap, &values[0]));
    assert(binary_heap_pop(heap, &out));
    assert(out == &values[0]);
    binary_heap_destroy(heap);
}

static void test_null_container(void) {
    int value = 1;
    void *out = &value;

    assert(!binary_heap_push(NULL, &value));
    assert(!binary_heap_pop(NULL, &out));
    assert(!binary_heap_peek(NULL, &out));
    assert(out == &value);
    assert(binary_heap_size(NULL) == 0U);
    assert(binary_heap_is_empty(NULL));
    binary_heap_destroy(NULL);
}

int main(void) {
    test_empty_heap();
    test_orders_by_comparison();
    test_interleaved_push_pop();
    test_growth_and_reuse();
    test_null_container();
    return 0;
}
