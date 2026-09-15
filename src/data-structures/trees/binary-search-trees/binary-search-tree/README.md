# Binary Search Tree

An unbalanced opaque binary search tree of unique, borrowed pointers. Each node's left subtree compares lower and its right subtree higher.

## C API
```c
typedef int (*BinarySearchTreeCompareFn)(const void *left, const void *right); typedef bool (*BinarySearchTreeVisitFn)(void *item, void *context);
BinarySearchTree *binary_search_tree_create(BinarySearchTreeCompareFn compare); void binary_search_tree_destroy(BinarySearchTree *tree);
bool binary_search_tree_insert(BinarySearchTree *tree, void *item); bool binary_search_tree_find(const BinarySearchTree *tree, const void *key, void **out_item);
bool binary_search_tree_contains(const BinarySearchTree *tree, const void *key); bool binary_search_tree_remove(BinarySearchTree *tree, const void *key, void **out_item);
bool binary_search_tree_in_order(const BinarySearchTree *tree, BinarySearchTreeVisitFn visit, void *context); size_t binary_search_tree_size(const BinarySearchTree *tree); bool binary_search_tree_is_empty(const BinarySearchTree *tree);
bool binary_search_tree_graph_view(const BinarySearchTree *tree, GraphView *out_view);
```

## Behavior, Ownership, and Invariants
- Create requires a comparator; inserted items and lookup keys cannot be null. Equal inserts fail and preserve the first stored pointer. Find/remove outputs change only on success.
- Removal returns the logical removed value; a two-child node is replaced by its in-order successor. In-order visits ascending values and returns false when its visitor stops; an empty valid tree succeeds.
- The tree owns nodes but never items. Its borrowed GraphView indexes nodes in-order and exposes directed parent-to-child unit edges. It expires on tree destruction or structural mutation.

## Complexity and Verification
Operations are O(h): average O(log n), worst O(n); traversal is O(n), with O(h) recursion. Verify with `make test NAME=data-structures/trees/binary-search-trees/binary-search-tree`.
