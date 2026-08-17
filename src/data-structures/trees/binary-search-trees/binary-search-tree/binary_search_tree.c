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

// Bundles traversal configuration so one walker serves destroy and in-order.
struct Options {
    // Called once per node in traversal order.
    TraversalFunction action;
    // Caller-supplied visitor forwarded during in-order traversal.
    BinarySearchTreeVisitFn visit;
    // Caller-supplied context forwarded to the visitor unchanged.
    void *context;
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

// Walks the subtree in left-value-right order, applying the configured action.
bool traverse_tree(Node *node, Options *options) {
    // Stops recursing below a missing child; callers short-circuit the empty
    // tree before reaching this walker, so false here never reports failure
    // for a legitimately empty traversal.
    if (node == NULL) {
        return false;
    }

    // Visits every lower-valued node before this node.
    if (node->left != NULL) {
        if (!traverse_tree(node->left, options)) {
            return false;
        }
    }

    // saves a temp variable incase action frees up the memory of the node
    Node *right = node->right;
    bool result = true;
    
    // Applies the configured action only when traversal options were provided.
    if (options != NULL) {
        if (options->action != NULL) {
            // Invokes the configured action with the node and shared options.
            result = options->action(node, options);
            // Stops the walk when the action asks to halt early.
            if (result == false) {
                return false;
            }
        }
    }

    // Visits every higher-valued node after this node.
    if (right != NULL) {
        if (!traverse_tree(right, options)) {
            return false;
        }
    }
    
    // Reports that this subtree finished without an early stop.
    return !!result;
}

// Frees one node during the destroy traversal.
bool traversal_freedom(Node *node, Options *options) {
    // Releases the tree-owned node while leaving the caller-owned value alone.
    free(node);
    // Marks the unused options parameter as intentionally ignored.
    (void)(options);
    // Continues the traversal after freeing this node.
    return true;
}

// Releases nodes owned by the tree wrapper.
void binary_search_tree_destroy(BinarySearchTree *tree) {
    // Makes destruction of a null tree a safe no-op.
    if (tree == NULL) {
        // Returns because there is no allocation to release.
        return;
    }

    // Configures the shared walker to free each node it visits.
    Options options = { .action = traversal_freedom };

    // Frees every node in left-value-right order before the wrapper.
    traverse_tree(tree->root, &options);
    // Frees the tree wrapper after its nodes are gone.
    free(tree);

    // Ends the void function.
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
        // Rejects a missing lookup key.
        return false;
    }

    if (out_item == NULL) {
        // Reports failure because a successful removal must return the pointer.
        return false;
    }

    // Tracks the parent so a matched node can be unlinked from it.
    Node *previous = NULL;
    // Starts the comparison-guided search at the root.
    Node *current = tree->root;

    // Follows one branch per node until a match or a dead end.
    while(current != NULL) {
        // Compares the current stored item against the caller's removal key.
        int direction = tree->compare(current->value, key);
        if (direction > 0) {
            // Reports absence when the required lower child does not exist.
            if (current->left == NULL) {
                return false;
            }

            // Descends left, remembering the parent.
            previous = current;
            current = current->left;
        } else if (direction < 0) {
            // Reports absence when the required higher child does not exist.
            if (current->right == NULL) {
                return false;
            }

            // Descends right, remembering the parent.
            previous = current;
            current = current->right;
        } else {
            // Copies the stored pointer out before any node is freed.
            *out_item = current->value;
            if (previous == NULL) {
                // Handles the root, whose incoming link is tree->root rather
                // than a parent's child pointer.
                if (current->left == NULL && current->right == NULL) {
                    // Empties the tree when the root is also a leaf.
                    tree->root = NULL;
                    // Uncounts the removed item.
                    tree->size--;
                    // Frees the tree-owned node, never the caller's value.
                    free(current);
                    // Reports successful root-leaf removal.
                    return true;
                } else if (current->right == NULL) {
                    // Promotes the only (left) child to become the new root.
                    tree->root = current->left;
                    // Uncounts the removed item.
                    tree->size--;
                    // Frees the bypassed node.
                    free(current);
                    // Reports successful one-child root removal.
                    return true;
                } else if (current->left == NULL) {
                    // Promotes the only (right) child to become the new root.
                    tree->root = current->right;
                    // Uncounts the removed item.
                    tree->size--;
                    // Frees the bypassed node.
                    free(current);
                    // Reports successful one-child root removal.
                    return true;
                } else {
                    // Replaces the root's value with its in-order successor from
                    // the right subtree.
                    Node *prev_in_order_node = current;
                    Node *in_order_node = current->right;

                    // Descends to the leftmost node of the right subtree while
                    // keeping its parent one step behind.
                    while(in_order_node->left != NULL) {
                        prev_in_order_node = in_order_node;
                        in_order_node = in_order_node->left;
                    }

                    // Reattaches the successor's right subtree on whichever side
                    // of its parent held the successor.
                    if (prev_in_order_node == current) {
                        prev_in_order_node->right = in_order_node->right;
                    } else {
                        prev_in_order_node->left = in_order_node->right;
                    }

                    // Moves the successor's value into the root node.
                    current->value = in_order_node->value;
                    // Uncounts the removed item.
                    tree->size--;
                    // Frees the vacated successor node.
                    free(in_order_node);
                    // Reports successful two-child root removal.
                    return true;
                }
            } else {
                // Determines which parent link points at the matched node.
                int previous_direction = tree->compare(previous->value, key);
                if (current->left == NULL && current->right == NULL) {
                    // Unlinks a leaf from whichever side of the parent held it.
                    if (previous_direction > 0) {
                        previous->left = NULL;
                    } else {
                        previous->right = NULL;
                    }

                    // Frees the tree-owned node, never the caller's value.
                    free(current);
                    // Uncounts the removed item.
                    tree->size--;
                    // Reports successful leaf removal.
                    return true;
                } else if (current->left == NULL) {
                    // Promotes the only (right) child into the parent's slot.
                    if (previous_direction > 0) {
                        previous->left = current->right;
                    } else {
                        previous->right = current->right;
                    }

                    // Frees the bypassed node.
                    free(current);
                    // Uncounts the removed item.
                    tree->size--;
                    // Reports successful one-child removal.
                    return true;
                } else if (current->right == NULL) {
                    // Promotes the only (left) child into the parent's slot.
                    if (previous_direction > 0) {
                        previous->left = current->left;
                    } else {
                        previous->right = current->left;
                    }
                    
                    // Frees the bypassed node.
                    free(current);
                    // Uncounts the removed item.
                    tree->size--;
                    // Reports successful one-child removal.
                    return true;
                } else {
                    // Replaces the matched value with its in-order successor from
                    // the right subtree.
                    Node *prev_in_order_node = current;
                    Node *in_order_node = current->right;
                    // Descends to the leftmost node of the right subtree while
                    // keeping its parent one step behind.
                    while(in_order_node->left != NULL) {
                        prev_in_order_node = in_order_node;
                        in_order_node = in_order_node->left;
                    }

                    // Reattaches the successor's right subtree on whichever side
                    // of its parent held the successor: the removed node links to
                    // it through ->right, a leftmost descendant through ->left.
                    if (prev_in_order_node == current) {
                        prev_in_order_node->right = in_order_node->right;
                    } else {
                        prev_in_order_node->left = in_order_node->right;
                    }

                    // Moves the successor's value into the matched node.
                    current->value = in_order_node->value;
                    // Uncounts the removed item.
                    tree->size--;
                    // Frees the vacated successor node.
                    free(in_order_node);
                    // Reports successful two-child removal.
                    return true;
                }
            }
        }
    }

    // Reports absence when the tree is empty.
    return false;
}

// Adapts the caller's visitor to the traversal action interface.
bool in_order_action(Node *node, Options *options) {
    // Guards against a missing node before dereferencing it.
    if (node == NULL) {
        return false;
    }

    // Normalizes the visitor's answer to a strict bool.
    return !!options->visit(node->value, options->context);
}

// Visits stored items in increasing comparison order.
bool binary_search_tree_in_order(const BinarySearchTree *tree, BinarySearchTreeVisitFn visit, void *context) {
    // Rejects a missing tree pointer.
    if (tree == NULL) {
        // Reports failure because there is no tree to traverse.
        return false;
    }

    if (visit == NULL) {
        // Rejects a missing visitor because traversal has no effect without one.
        return false;
    }

    // Reports success for an empty tree because zero visits complete trivially.
    if (tree->size == 0U) {
        return true;
    }

    // Configures the shared walker with the caller's visitor and context.
    Options options = { .action = in_order_action, .visit = visit, .context = context };

    // Walks the whole tree in increasing comparison order.
    return traverse_tree(tree->root, &options);
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
