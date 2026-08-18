#include "doubly_linked_list.h"

#include <assert.h>
#include <stddef.h>

struct Record {
    int id;
};

static void test_empty_list(void) {
    DoublyLinkedList *list = doubly_linked_list_create();
    int sentinel = 0;
    void *out = &sentinel;

    assert(list != NULL);
    assert(doubly_linked_list_size(list) == 0U);
    assert(doubly_linked_list_is_empty(list));
    assert(!doubly_linked_list_pop_front(list, &out));
    assert(out == &sentinel);
    assert(!doubly_linked_list_pop_back(list, &out));
    assert(out == &sentinel);
    assert(!doubly_linked_list_get(list, 0U, &out));
    assert(out == &sentinel);
    assert(!doubly_linked_list_remove(list, 0U, &out));
    assert(out == &sentinel);
    assert(!doubly_linked_list_pop_front(list, NULL));
    assert(!doubly_linked_list_pop_back(list, NULL));
    doubly_linked_list_destroy(list);
}

static void test_insert_remove_and_generic_values(void) {
    DoublyLinkedList *list = doubly_linked_list_create();
    int first = 1;
    int second = 2;
    struct Record record = { .id = 3 };
    void *out = NULL;

    assert(doubly_linked_list_push_front(list, &second));
    assert(doubly_linked_list_push_front(list, &first));
    assert(doubly_linked_list_push_back(list, NULL));
    assert(doubly_linked_list_insert(list, 2U, &record));
    assert(!doubly_linked_list_insert(list, 5U, &first));
    assert(doubly_linked_list_size(list) == 4U);

    assert(doubly_linked_list_get(list, 0U, &out));
    assert(out == &first);
    assert(doubly_linked_list_get(list, 1U, &out));
    assert(out == &second);
    assert(doubly_linked_list_get(list, 2U, &out));
    assert(out == &record);
    assert(doubly_linked_list_get(list, 3U, &out));
    assert(out == NULL);

    assert(doubly_linked_list_remove(list, 1U, &out));
    assert(out == &second);
    assert(doubly_linked_list_pop_front(list, &out));
    assert(out == &first);
    assert(doubly_linked_list_pop_back(list, &out));
    assert(out == NULL);
    assert(doubly_linked_list_pop_back(list, &out));
    assert(out == &record);
    assert(doubly_linked_list_is_empty(list));
    doubly_linked_list_destroy(list);
}

static void test_interior_insert_lands_at_index(void) {
    DoublyLinkedList *list = doubly_linked_list_create();
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    int x = 9;
    void *out = NULL;

    assert(doubly_linked_list_push_back(list, &a));
    assert(doubly_linked_list_push_back(list, &b));
    assert(doubly_linked_list_push_back(list, &c));
    assert(doubly_linked_list_push_back(list, &d));

    assert(doubly_linked_list_insert(list, 2U, &x));
    assert(doubly_linked_list_size(list) == 5U);
    assert(doubly_linked_list_get(list, 1U, &out));
    assert(out == &b);
    assert(doubly_linked_list_get(list, 2U, &out));
    assert(out == &x);
    assert(doubly_linked_list_get(list, 3U, &out));
    assert(out == &c);

    assert(doubly_linked_list_insert(list, 4U, &x));
    assert(doubly_linked_list_get(list, 4U, &out));
    assert(out == &x);
    assert(doubly_linked_list_get(list, 5U, &out));
    assert(out == &d);
    doubly_linked_list_destroy(list);
}

static void test_remove_syncs_both_ends(void) {
    DoublyLinkedList *list = doubly_linked_list_create();
    int a = 1;
    int b = 2;
    int c = 3;
    void *out = NULL;

    assert(doubly_linked_list_push_back(list, &a));
    assert(doubly_linked_list_push_back(list, &b));
    assert(doubly_linked_list_push_back(list, &c));

    assert(doubly_linked_list_remove(list, 2U, &out));
    assert(out == &c);
    assert(doubly_linked_list_push_back(list, &c));
    assert(doubly_linked_list_get(list, 2U, &out));
    assert(out == &c);

    assert(doubly_linked_list_remove(list, 0U, &out));
    assert(out == &a);
    assert(doubly_linked_list_push_front(list, &a));
    assert(doubly_linked_list_get(list, 0U, &out));
    assert(out == &a);

    assert(doubly_linked_list_remove(list, 2U, &out));
    assert(doubly_linked_list_remove(list, 1U, &out));
    assert(doubly_linked_list_remove(list, 0U, &out));
    assert(doubly_linked_list_is_empty(list));

    assert(doubly_linked_list_push_back(list, &b));
    assert(doubly_linked_list_pop_front(list, &out));
    assert(out == &b);
    assert(doubly_linked_list_push_front(list, &b));
    assert(doubly_linked_list_pop_back(list, &out));
    assert(out == &b);
    assert(doubly_linked_list_is_empty(list));
    doubly_linked_list_destroy(list);
}

static void test_large_list_and_reuse(void) {
    enum { ITEM_COUNT = 1024 };
    DoublyLinkedList *list = doubly_linked_list_create();
    int values[ITEM_COUNT];
    void *out = NULL;

    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        values[index] = (int)index;
        assert(doubly_linked_list_push_back(list, &values[index]));
    }
    for (size_t index = 0U; index < ITEM_COUNT; ++index) {
        assert(doubly_linked_list_get(list, index, &out));
        assert(out == &values[index]);
    }
    for (size_t index = ITEM_COUNT; index > 0U; --index) {
        assert(doubly_linked_list_pop_back(list, &out));
        assert(out == &values[index - 1U]);
    }
    assert(doubly_linked_list_is_empty(list));
    assert(doubly_linked_list_push_back(list, &values[0]));
    assert(doubly_linked_list_pop_front(list, &out));
    assert(out == &values[0]);
    doubly_linked_list_destroy(list);
}

static void test_index_boundaries(void) {
    DoublyLinkedList *list = doubly_linked_list_create();
    int first = 1;
    int second = 2;
    int sentinel = 0;
    void *out = &sentinel;

    assert(list != NULL);
    assert(doubly_linked_list_insert(list, 0U, &first));
    assert(doubly_linked_list_get(list, 0U, &out));
    assert(out == &first);
    assert(doubly_linked_list_insert(list, 1U, &second));
    assert(doubly_linked_list_get(list, 1U, &out));
    assert(out == &second);
    assert(!doubly_linked_list_insert(list, 3U, &first));
    out = &sentinel;
    assert(!doubly_linked_list_get(list, 2U, &out));
    assert(out == &sentinel);
    assert(!doubly_linked_list_remove(list, 2U, &out));
    assert(out == &sentinel);
    assert(doubly_linked_list_remove(list, 1U, &out));
    assert(out == &second);
    assert(doubly_linked_list_remove(list, 0U, &out));
    assert(out == &first);
    assert(doubly_linked_list_is_empty(list));
    doubly_linked_list_destroy(list);
}

static void test_null_container(void) {
    int value = 1;
    void *out = &value;

    assert(!doubly_linked_list_push_front(NULL, &value));
    assert(!doubly_linked_list_push_back(NULL, &value));
    assert(!doubly_linked_list_get(NULL, 0U, &out));
    assert(!doubly_linked_list_insert(NULL, 0U, &value));
    assert(!doubly_linked_list_remove(NULL, 0U, &out));
    assert(out == &value);
    assert(doubly_linked_list_size(NULL) == 0U);
    assert(doubly_linked_list_is_empty(NULL));
    doubly_linked_list_destroy(NULL);
}

int main(void) {
    test_empty_list();
    test_insert_remove_and_generic_values();
    test_interior_insert_lands_at_index();
    test_remove_syncs_both_ends();
    test_large_list_and_reuse();
    test_index_boundaries();
    test_null_container();
    return 0;
}
