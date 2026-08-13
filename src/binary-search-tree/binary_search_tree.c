// Provides the public BinarySearchTree type and function declarations.
#include "binary_search_tree.h"
// Declares malloc and free.
#include <stdlib.h>

// Defines one node in the tree.
struct Node {
    // Stores the caller-owned item pointer.
    void *value;
    // Points to items that compare lower than this node's value.
    Node *left;
    // Points to items that compare higher than this node's value.
    Node *right;
};

// Defines the fields hidden from callers of the public API.
struct BinarySearchTree {
    // Counts values stored in the tree.
    size_t size;
    // Points to the top node, or NULL when the tree is empty.
    Node *root;
    // Compares caller-owned values and lookup keys.
    BinarySearchTreeCompareFn compare;
};

// Allocates an empty tree with caller-supplied ordering behavior.
BinarySearchTree *binary_search_tree_create(BinarySearchTreeCompareFn compare) {
    // Rejects a missing comparison function because ordering is required.
    if (compare == NULL) {
        // Reports invalid configuration to the caller.
        return NULL;
    }

    // Allocates the tree wrapper before any nodes exist.
    BinarySearchTree *tree = malloc(sizeof(BinarySearchTree));

    // Stops before accessing the wrapper when allocation fails.
    if (tree == NULL) {
        // Reports allocation failure to the caller.
        return NULL;
    }

    // Records that no values have been inserted yet.
    tree->size = 0U;
    // Starts with no root node.
    tree->root = NULL;
    // Stores the caller's comparison function for future tree operations.
    tree->compare = compare;

    // Returns the fully initialized empty tree.
    return tree;
}

// Releases nodes owned by the tree wrapper.
void binary_search_tree_destroy(BinarySearchTree *tree) {
    // Makes destruction of a null tree a safe no-op.
    if (tree == NULL) {
        // Returns because there is no allocation to release.
        return;
    }

    // TODO: Traverse and free every node without freeing caller-owned values.

    return;
}

// Inserts a caller-owned item according to the tree's comparison function.
bool binary_search_tree_insert(BinarySearchTree *tree, void *item) {
    // Rejects a missing tree pointer.
    if (tree == NULL) {
        // Reports failure because there is no tree to modify.
        return false;
    }

    if (item == NULL) {
        // Rejects null items because the tree contract disallows them.
        return false;
    }

    // Handles the special case where the first item becomes the root node.
    if (tree->size == 0U) {
        // Allocates the first table-owned node.
        Node *node = malloc(sizeof(Node));
        // Stops before accessing the node when allocation fails.
        if (node == NULL) {
            // Leaves the empty tree unchanged on allocation failure.
            return false;
        }

        // Stores the caller-owned item pointer in the new root node.
        node->value = item;
        // Starts the root with no lower-valued child.
        node->left = NULL;
        // Starts the root with no higher-valued child.
        node->right = NULL;
        // Connects the first node as the tree root.
        tree->root = node;
        // Counts the newly inserted item.
        tree->size++;
        // Reports successful root insertion.
        return true;
    }

    // Starts at the existing root to follow one comparison-guided path.
    Node *temp = tree->root;
    // Continues until inserting a new leaf or finding a duplicate.
    while (temp != NULL) {
        // Compares the current item with the caller's new item.
        int direction = tree->compare(temp->value, item);
        if (direction > 0) {
            // Uses the lower-valued branch because the new item sorts before current.
            if (temp->left == NULL) {
                // Allocates a leaf where the lower-valued child belongs.
                Node *left = malloc(sizeof(Node));
                // Stops without changing the tree when allocation fails.
                if (left == NULL) {
                    return false;
                }

                // Stores the caller-owned item pointer in the new leaf.
                left->value = item;
                // Starts the new leaf without children.
                left->left = NULL;
                left->right = NULL;
                // Links the new leaf below the current node.
                temp->left = left;
                // Counts the newly inserted item.
                tree->size++;
                // Reports successful lower-branch insertion.
                return true;
            } else {
                // Continues down the existing lower-valued branch.
                temp = temp->left;
            }
        } else if (direction < 0) {
            // Uses the higher-valued branch because the new item sorts after current.
            if (temp->right == NULL) {
                // Allocates a leaf where the higher-valued child belongs.
                Node *right = malloc(sizeof(Node));
                // Stops without changing the tree when allocation fails.
                if (right == NULL) {
                    return false;
                }

                // Stores the caller-owned item pointer in the new leaf.
                right->value = item;
                // Starts the new leaf without children.
                right->left = NULL;
                right->right = NULL;
                // Links the new leaf below the current node.
                temp->right = right;
                // Counts the newly inserted item.
                tree->size++;
                // Reports successful higher-branch insertion.
                return true;
            } else {
                // Continues down the existing higher-valued branch.
                temp = temp->right;
            }
        } else {
            // Preserves the first stored pointer when the new item compares equal.
            return false;
        }
    }

    // Defensively reports failure if a broken tree invariant ends the loop.
    return false;
}

// Looks up the stored item matching a caller-supplied comparison key.
bool binary_search_tree_find(const BinarySearchTree *tree, const void *key, void **out_item) {
    // Rejects a missing tree pointer.
    if (tree == NULL) {
        // Reports failure because there is no tree to search.
        return false;
    }

    if (key == NULL) {
        // Leaves the caller's output pointer unchanged for an invalid key.
        return false;
    }

    // Rejects a missing location for the stored item pointer.
    if (out_item == NULL) {
        // Reports failure because a successful lookup needs an output location.
        return false;
    }

    // Starts at the root of the comparison-guided search path.
    Node *temp = tree->root;
    // Follows one branch at each node until finding a match or reaching a missing child.
    while (temp != NULL) {
        // Compares the current stored item against the caller's lookup key.
        int direction = tree->compare(temp->value, key);
        if (direction > 0) {
            // Searches left because the lookup key is lower than the current item.
            if (temp->left == NULL) {
                // Reports absence when the required child does not exist.
                return false;
            } else {
                // Continues at the lower-valued child.
                temp = temp->left;
            }
        } else if (direction < 0) {
            // Searches right because the lookup key is higher than the current item.
            if (temp->right == NULL) {
                // Reports absence when the required child does not exist.
                return false;
            } else {
                // Continues at the higher-valued child.
                temp = temp->right;
            }
        } else {
            // Copies the original caller-owned stored pointer to the output location.
            *out_item = temp->value;
            // Reports the successful lookup.
            return true;
        }
    }

    // Reports absence when the tree has no root or the search path ends.
    return false;
}

// Reports whether a caller-supplied comparison key is present.
bool binary_search_tree_contains(const BinarySearchTree *tree, const void *key) {
    // Rejects a missing tree pointer.
    if (tree == NULL) {
        // Reports that no key can be present.
        return false;
    }

    // Provides find with a valid output location that contains does not otherwise need.
    void *out_item = NULL;

    // Reuses comparison-guided lookup and reports only whether it found a match.
    return binary_search_tree_find(tree, key, &out_item);
}

// Removes the stored item matching a caller-supplied comparison key.
bool binary_search_tree_remove(BinarySearchTree *tree, const void *key, void **out_item) {
    // Rejects a missing tree pointer.
    if (tree == NULL) {
        // Reports failure because there is no tree to modify.
        return false;
    }

    if (key == NULL) {
        return false;
    }

    if (out_item == NULL) {
        return false;
    }

    // TODO: Reject NULL keys/output and handle leaf, one-child, two-child, and root removal.
    Node *current = tree->root;
    Node *previous = NULL;
    while(current != NULL) {
        int direction = tree->compare(current->value, key);
        if (direction > 0) {
            if (current->left == NULL) {
                return false;
            } else {
                previous = current;
                current = current->left;
            }
        } else if (direction < 0) {
            if (current->right == NULL) {
                return false;
            } else {
                previous = current;
                current = current->right;
            }
        } else {
            *out_item = current->value;
            // need to remove the node and set the falling leaves back in the new correct spots
            int old_direction = tree->compare(previous->value, key);
            if (current->left == NULL && current->right == NULL) {
                if (old_direction < 0) {
                    previous->left = current->left;
                } else {
                    previous->right = current->right;
                }
            }

            // TODO: the rest of actually removign the node

            tree->size--;
            return true;
        }
    }

    // Reports absence until removal is implemented.
    return false;
}

// Visits stored items in increasing comparison order.
bool binary_search_tree_in_order(const BinarySearchTree *tree, BinarySearchTreeVisitFn visit, void *context) {
    // Rejects a missing tree pointer.
    if (tree == NULL) {
        // Reports failure because there is no tree to traverse.
        return false;
    }

    // TODO: Reject NULL visitors, recursively visit left subtree, current item, then right subtree.
    (void)(visit);
    (void)(context);

    // Reports failure until traversal is implemented.
    return false;
}

// Returns the tracked number of values without traversing the tree.
size_t binary_search_tree_size(const BinarySearchTree *tree) {
    // Treats a null tree as containing zero values.
    if (tree == NULL) {
        // Returns the empty-tree count.
        return 0U;
    }

    // Returns the constant-time value count.
    return tree->size;
}

// Reports whether a tree contains no values.
bool binary_search_tree_is_empty(const BinarySearchTree *tree) {
    // Treats a null tree as empty under this API contract.
    if (tree == NULL) {
        // Reports an empty tree.
        return true;
    }

    // Uses the constant-time value count to report emptiness.
    return tree->size == 0U;
}
