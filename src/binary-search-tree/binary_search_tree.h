#include <stdbool.h>
#include <stddef.h>

typedef struct Node Node;
typedef struct BinarySearchTree BinarySearchTree;
typedef int (*BinarySearchTreeCompareFn)(const void *left, const void *right);
typedef bool (*BinarySearchTreeVisitFn)(void *item, void *context);

BinarySearchTree *binary_search_tree_create(BinarySearchTreeCompareFn compare);
void binary_search_tree_destroy(BinarySearchTree *tree);
bool binary_search_tree_insert(BinarySearchTree *tree, void *item);
bool binary_search_tree_find(const BinarySearchTree *tree, const void *key, void **out_item);
bool binary_search_tree_contains(const BinarySearchTree *tree, const void *key);
bool binary_search_tree_remove(BinarySearchTree *tree, const void *key, void **out_item);
bool binary_search_tree_in_order(const BinarySearchTree *tree, BinarySearchTreeVisitFn visit, void *context);
size_t binary_search_tree_size(const BinarySearchTree *tree);
bool binary_search_tree_is_empty(const BinarySearchTree *tree);
