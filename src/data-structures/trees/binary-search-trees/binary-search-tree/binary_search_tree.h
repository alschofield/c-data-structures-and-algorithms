// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Names the private BST node type.
typedef struct Node Node;
// Names private traversal options used by implementation helpers.
typedef struct Options Options;
// Names the opaque binary search tree.
typedef struct BinarySearchTree BinarySearchTree;
// Compares two caller-owned values using qsort-style ordering.
typedef int (*BinarySearchTreeCompareFn)(const void *left, const void *right);
// Visits one in-order item and returns false to stop traversal.
typedef bool (*BinarySearchTreeVisitFn)(void *item, void *context);
// Names an internal node-traversal helper callback.
typedef bool (*TraversalFunction)(Node *node, Options *options);

// Allocates an empty tree with its ordering callback.
BinarySearchTree *binary_search_tree_create(BinarySearchTreeCompareFn compare);
// Frees tree nodes without freeing caller-owned values.
void binary_search_tree_destroy(BinarySearchTree *tree);
// Inserts one unique caller-owned item.
bool binary_search_tree_insert(BinarySearchTree *tree, void *item);
// Finds and returns the stored item equal to key.
bool binary_search_tree_find(const BinarySearchTree *tree, const void *key, void **out_item);
// Reports whether key exists in the tree.
bool binary_search_tree_contains(const BinarySearchTree *tree, const void *key);
// Removes and returns the stored item equal to key.
bool binary_search_tree_remove(BinarySearchTree *tree, const void *key, void **out_item);
// Visits stored items in ascending comparison order.
bool binary_search_tree_in_order(const BinarySearchTree *tree, BinarySearchTreeVisitFn visit, void *context);
// Returns the number of stored items.
size_t binary_search_tree_size(const BinarySearchTree *tree);
// Reports whether the tree contains no items.
bool binary_search_tree_is_empty(const BinarySearchTree *tree);
