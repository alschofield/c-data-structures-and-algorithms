# Binary Search Tree

Generic ordered collection of caller-owned, non-`NULL` values. The caller
provides the comparison function that defines the tree order.

## How It Works

Binary search as a shape. Every node obeys one invariant: all values in its
left subtree order before it, all in its right subtree after. Lookup is a
root-to-leaf descent discarding half the remaining tree per comparison —
O(log n) when the tree is balanced, O(n) when sorted insertions degenerate
it into a chain. In-order traversal (left, node, right) visits values in
sorted order. Removal is the structural exercise: leaves detach, one-child
nodes promote the child, and two-child nodes swap in their in-order
successor — the smallest value of the right subtree — before deleting it
from where it was.

## Required API

```c
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
```

## Contract

- `compare(left, right)` follows `qsort` semantics: negative, zero, or positive.
- Equal values are duplicates: `insert` returns `false` and preserves the first
  stored pointer.
- `find` and `remove` use a comparison key and return the stored pointer.
- `remove` must work for leaf nodes, nodes with one child, nodes with two
  children, and the root.
- In-order traversal visits values in strictly increasing comparison order.
  It stops and returns `false` if the visitor returns `false`.
- `destroy` frees nodes only, never values.

## Complexity Targets

- Balanced tree: `insert`, `find`, `remove`, `contains`: O(log n)
- This unbalanced BST: worst case O(n)
- `in_order`: O(n)
- Space: O(n) nodes plus O(height) working space for traversal/removal
