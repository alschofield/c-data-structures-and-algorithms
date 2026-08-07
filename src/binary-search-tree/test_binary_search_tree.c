#include "binary_search_tree.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

struct Record {
    int id;
};

struct VisitLog {
    void *items[16];
    size_t count;
    size_t limit;
};

static int record_compare(const void *left, const void *right) {
    const struct Record *left_record = left;
    const struct Record *right_record = right;

    return (left_record->id > right_record->id) - (left_record->id < right_record->id);
}

static bool collect_visit(void *item, void *context) {
    struct VisitLog *log = context;

    assert(log->count < sizeof(log->items) / sizeof(log->items[0]));
    log->items[log->count] = item;
    ++log->count;
    return log->count < log->limit;
}

static void test_creation_and_empty_tree(void) {
    BinarySearchTree *tree;
    struct Record key = { .id = 1 };
    int sentinel = 0;
    void *out = &sentinel;
    struct VisitLog log = { .limit = 16U };

    assert(binary_search_tree_create(NULL) == NULL);
    tree = binary_search_tree_create(record_compare);
    assert(tree != NULL);
    assert(binary_search_tree_size(tree) == 0U);
    assert(binary_search_tree_is_empty(tree));
    assert(!binary_search_tree_find(tree, &key, &out));
    assert(out == &sentinel);
    assert(!binary_search_tree_remove(tree, &key, &out));
    assert(out == &sentinel);
    assert(!binary_search_tree_find(tree, &key, NULL));
    assert(!binary_search_tree_remove(tree, &key, NULL));
    assert(binary_search_tree_in_order(tree, collect_visit, &log));
    assert(log.count == 0U);
    assert(!binary_search_tree_in_order(tree, NULL, &log));
    binary_search_tree_destroy(tree);
}

static void test_insert_find_and_in_order_traversal(void) {
    BinarySearchTree *tree = binary_search_tree_create(record_compare);
    struct Record records[] = {
        { .id = 4 }, { .id = 2 }, { .id = 6 }, { .id = 1 },
        { .id = 3 }, { .id = 5 }, { .id = 7 }
    };
    struct Record duplicate = { .id = 4 };
    struct Record key = { .id = 3 };
    void *out = NULL;
    struct VisitLog complete_log = { .limit = 16U };
    struct VisitLog stopped_log = { .limit = 3U };
    void *expected[] = {
        &records[3], &records[1], &records[4], &records[0],
        &records[5], &records[2], &records[6]
    };

    for (size_t index = 0U; index < sizeof(records) / sizeof(records[0]); ++index) {
        assert(binary_search_tree_insert(tree, &records[index]));
    }
    assert(!binary_search_tree_insert(tree, &duplicate));
    assert(!binary_search_tree_insert(tree, NULL));
    assert(binary_search_tree_size(tree) == 7U);
    assert(binary_search_tree_contains(tree, &key));
    assert(binary_search_tree_find(tree, &key, &out));
    assert(out == &records[4]);

    assert(binary_search_tree_in_order(tree, collect_visit, &complete_log));
    assert(complete_log.count == 7U);
    for (size_t index = 0U; index < complete_log.count; ++index) {
        assert(complete_log.items[index] == expected[index]);
    }
    assert(!binary_search_tree_in_order(tree, collect_visit, &stopped_log));
    assert(stopped_log.count == 3U);
    binary_search_tree_destroy(tree);
}

static void test_leaf_one_child_two_children_and_root_removal(void) {
    BinarySearchTree *tree = binary_search_tree_create(record_compare);
    struct Record records[] = {
        { .id = 4 }, { .id = 2 }, { .id = 6 }, { .id = 1 },
        { .id = 3 }, { .id = 5 }, { .id = 7 }
    };
    struct Record key = { .id = 0 };
    void *out = NULL;

    for (size_t index = 0U; index < sizeof(records) / sizeof(records[0]); ++index) {
        assert(binary_search_tree_insert(tree, &records[index]));
    }

    key.id = 1;
    assert(binary_search_tree_remove(tree, &key, &out));
    assert(out == &records[3]);
    assert(!binary_search_tree_contains(tree, &key));

    key.id = 2;
    assert(binary_search_tree_remove(tree, &key, &out));
    assert(out == &records[1]);
    assert(!binary_search_tree_contains(tree, &key));

    key.id = 6;
    assert(binary_search_tree_remove(tree, &key, &out));
    assert(out == &records[2]);
    assert(!binary_search_tree_contains(tree, &key));
    key.id = 5;
    assert(binary_search_tree_contains(tree, &key));
    key.id = 7;
    assert(binary_search_tree_contains(tree, &key));

    key.id = 4;
    assert(binary_search_tree_remove(tree, &key, &out));
    assert(out == &records[0]);
    assert(!binary_search_tree_contains(tree, &key));
    assert(binary_search_tree_size(tree) == 3U);
    binary_search_tree_destroy(tree);
}

static void test_null_container(void) {
    struct Record record = { .id = 1 };
    void *out = &record;
    struct VisitLog log = { .limit = 1U };

    assert(!binary_search_tree_insert(NULL, &record));
    assert(!binary_search_tree_find(NULL, &record, &out));
    assert(!binary_search_tree_remove(NULL, &record, &out));
    assert(out == &record);
    assert(!binary_search_tree_contains(NULL, &record));
    assert(!binary_search_tree_in_order(NULL, collect_visit, &log));
    assert(binary_search_tree_size(NULL) == 0U);
    assert(binary_search_tree_is_empty(NULL));
    binary_search_tree_destroy(NULL);
}

int main(void) {
    test_creation_and_empty_tree();
    test_insert_find_and_in_order_traversal();
    test_leaf_one_child_two_children_and_root_removal();
    test_null_container();
    return 0;
}
