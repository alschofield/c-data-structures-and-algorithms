#include "queue.h"

#include <assert.h>
#include <stddef.h>

struct Record {
    int id;
};

static void test_empty_queue(void) {
    Queue *queue = queue_create();
    int sentinel = 0;
    void *out = &sentinel;

    assert(queue != NULL);
    assert(queue_size(queue) == 0U);
    assert(queue_is_empty(queue));
    assert(!queue_dequeue(queue, &out));
    assert(out == &sentinel);
    assert(!queue_peek(queue, &out));
    assert(out == &sentinel);
    assert(!queue_dequeue(queue, NULL));
    assert(!queue_peek(queue, NULL));
    queue_destroy(queue);
}

static void test_fifo_and_generic_values(void) {
    Queue *queue = queue_create();
    int number = 42;
    struct Record record = { .id = 7 };
    const char label[] = "queue";
    void *out = NULL;

    assert(queue_enqueue(queue, &number));
    assert(queue_enqueue(queue, &record));
    assert(queue_enqueue(queue, (void *)label));
    assert(queue_enqueue(queue, NULL));
    assert(queue_size(queue) == 4U);
    assert(queue_peek(queue, &out));
    assert(out == &number);
    assert(queue_size(queue) == 4U);

    assert(queue_dequeue(queue, &out));
    assert(out == &number);
    assert(queue_dequeue(queue, &out));
    assert(out == &record);
    assert(queue_dequeue(queue, &out));
    assert(out == label);
    assert(queue_dequeue(queue, &out));
    assert(out == NULL);
    assert(queue_is_empty(queue));
    queue_destroy(queue);
}

static void test_growth_and_reuse(void) {
    enum { ITEM_COUNT = 1024 };
    Queue *queue = queue_create();
    int values[ITEM_COUNT];
    void *out = NULL;

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        values[index] = (int)index;
        assert(queue_enqueue(queue, &values[index]));
    }
    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        assert(queue_dequeue(queue, &out));
        assert(out == &values[index]);
    }
    assert(queue_is_empty(queue));
    assert(queue_enqueue(queue, &values[0]));
    assert(queue_dequeue(queue, &out));
    assert(out == &values[0]);
    queue_destroy(queue);
}

static void test_wraparound_growth(void) {
    Queue *queue = queue_create();
    int first = 1;
    int second = 2;
    int third = 3;
    int fourth = 4;
    void *out = NULL;

    assert(queue_enqueue(queue, &first));
    assert(queue_enqueue(queue, &second));
    assert(queue_dequeue(queue, &out));
    assert(out == &first);
    assert(queue_enqueue(queue, &third));
    assert(queue_enqueue(queue, &fourth));

    assert(queue_dequeue(queue, &out));
    assert(out == &second);
    assert(queue_dequeue(queue, &out));
    assert(out == &third);
    assert(queue_dequeue(queue, &out));
    assert(out == &fourth);
    assert(queue_is_empty(queue));
    queue_destroy(queue);
}

static void test_null_container(void) {
    int value = 1;
    void *out = &value;

    assert(!queue_enqueue(NULL, &value));
    assert(!queue_dequeue(NULL, &out));
    assert(out == &value);
    assert(!queue_peek(NULL, &out));
    assert(out == &value);
    assert(queue_size(NULL) == 0U);
    assert(queue_is_empty(NULL));
    queue_destroy(NULL);
}

int main(void) {
    test_empty_queue();
    test_fifo_and_generic_values();
    test_growth_and_reuse();
    test_wraparound_growth();
    test_null_container();
    return 0;
}
