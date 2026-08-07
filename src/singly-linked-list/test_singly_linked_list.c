#include "singly_linked_list.h"

#include <assert.h>
#include <stddef.h>

struct Record {
    int id;
};

static void test_empty_list(void) {
    SinglyLinkedList *list = singly_linked_list_create();
    int sentinel = 0;
    void *out = &sentinel;

    assert(list != NULL);
    assert(singly_linked_list_size(list) == 0U);
    assert(singly_linked_list_is_empty(list));
    assert(!singly_linked_list_pop_front(list, &out));
    assert(out == &sentinel);
    assert(!singly_linked_list_pop_back(list, &out));
    assert(out == &sentinel);
    assert(!singly_linked_list_get(list, 0U, &out));
    assert(out == &sentinel);
    assert(!singly_linked_list_pop_front(list, NULL));
    assert(!singly_linked_list_pop_back(list, NULL));
    singly_linked_list_destroy(list);
}

static void test_insert_remove_and_generic_values(void) {
    SinglyLinkedList *list = singly_linked_list_create();
    int first = 1;
    int second = 2;
    struct Record record = { .id = 3 };
    void *out = NULL;

    assert(singly_linked_list_push_front(list, &second));
    assert(singly_linked_list_push_front(list, &first));
    assert(singly_linked_list_push_back(list, NULL));
    assert(singly_linked_list_insert(list, 2U, &record));
    assert(!singly_linked_list_insert(list, 5U, &first));
    assert(singly_linked_list_size(list) == 4U);

    assert(singly_linked_list_get(list, 0U, &out));
    assert(out == &first);
    assert(singly_linked_list_get(list, 1U, &out));
    assert(out == &second);
    assert(singly_linked_list_get(list, 2U, &out));
    assert(out == &record);
    assert(singly_linked_list_get(list, 3U, &out));
    assert(out == NULL);

    assert(singly_linked_list_remove(list, 1U, &out));
    assert(out == &second);
    assert(singly_linked_list_pop_front(list, &out));
    assert(out == &first);
    assert(singly_linked_list_pop_back(list, &out));
    assert(out == NULL);
    assert(singly_linked_list_pop_back(list, &out));
    assert(out == &record);
    assert(singly_linked_list_is_empty(list));
    singly_linked_list_destroy(list);
}

static void test_large_list_and_reuse(void) {
    enum { ITEM_COUNT = 1024 };
    SinglyLinkedList *list = singly_linked_list_create();
    int values[ITEM_COUNT];
    void *out = NULL;

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        values[index] = (int)index;
        assert(singly_linked_list_push_back(list, &values[index]));
    }
    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        assert(singly_linked_list_get(list, index, &out));
        assert(out == &values[index]);
    }
    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        assert(singly_linked_list_pop_front(list, &out));
        assert(out == &values[index]);
    }
    assert(singly_linked_list_is_empty(list));
    assert(singly_linked_list_push_back(list, &values[0]));
    assert(singly_linked_list_pop_front(list, &out));
    assert(out == &values[0]);
    singly_linked_list_destroy(list);
}

static void test_index_boundaries(void) {
    SinglyLinkedList *list = singly_linked_list_create();
    int first = 1;
    int second = 2;
    int sentinel = 0;
    void *out = &sentinel;

    assert(list != NULL);
    assert(singly_linked_list_insert(list, 0U, &first));
    assert(singly_linked_list_get(list, 0U, &out));
    assert(out == &first);
    assert(singly_linked_list_insert(list, 1U, &second));
    assert(singly_linked_list_get(list, 1U, &out));
    assert(out == &second);
    assert(!singly_linked_list_insert(list, 3U, &first));
    out = &sentinel;
    assert(!singly_linked_list_get(list, 2U, &out));
    assert(out == &sentinel);
    assert(singly_linked_list_remove(list, 1U, &out));
    assert(out == &second);
    assert(singly_linked_list_remove(list, 0U, &out));
    assert(out == &first);
    assert(singly_linked_list_is_empty(list));
    singly_linked_list_destroy(list);
}

static void test_null_container(void) {
    int value = 1;
    void *out = &value;

    assert(!singly_linked_list_push_front(NULL, &value));
    assert(!singly_linked_list_push_back(NULL, &value));
    assert(!singly_linked_list_get(NULL, 0U, &out));
    assert(!singly_linked_list_insert(NULL, 0U, &value));
    assert(!singly_linked_list_remove(NULL, 0U, &out));
    assert(out == &value);
    assert(singly_linked_list_size(NULL) == 0U);
    assert(singly_linked_list_is_empty(NULL));
    singly_linked_list_destroy(NULL);
}

int main(void) {
    test_empty_list();
    test_insert_remove_and_generic_values();
    test_large_list_and_reuse();
    test_index_boundaries();
    test_null_container();
    return 0;
}
