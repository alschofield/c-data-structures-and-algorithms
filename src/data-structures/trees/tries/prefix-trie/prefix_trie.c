#include "prefix_trie.h"
// Declares malloc, realloc, and free.
#include <stdlib.h>
// Declares SIZE_MAX.
#include <stdint.h>

// Defines one character node in the sparse trie.
struct Node {
    // Marks whether a complete stored key ends at this node.
    bool is_end_of_word;
    // Stores the byte consumed from the parent to reach this node.
    char character;
    // Counts occupied child-pointer slots.
    size_t child_count;
    // Counts allocated child-pointer slots.
    size_t child_capacity;
    // Points to the growable sparse collection of child nodes.
    Node **children;
};

// Defines the fields hidden from callers of the public API.
struct PrefixTrie {
    // Counts complete stored keys, never character nodes.
    size_t size;
    // Points to the empty-prefix root node.
    Node *root;
};

// Allocates an empty trie and its empty-prefix root node.
PrefixTrie *prefix_trie_create(void) {
    // Allocates the public trie wrapper.
    PrefixTrie *trie = malloc(sizeof(PrefixTrie));

    // Reports wrapper allocation failure.
    if (trie == NULL) {
        return NULL;
    }

    // Allocates the root, which represents no character.
    Node *root = malloc(sizeof(Node));

    // Releases the wrapper when root allocation fails.
    if (root == NULL) {
        free(trie);
        return NULL;
    }

    // Initializes the root as a non-word with no child allocation.
    root->is_end_of_word = false;
    root->character = '\0';
    root->child_capacity = 0U;
    root->child_count = 0U;
    root->children = NULL;

    // Initializes the stored-key count and publishes the root.
    trie->size = 0U;
    trie->root = root;

    return trie;
}

// Frees every node and child-pointer array reachable from node.
static void recursively_delete(Node *node) {
    // Treats a missing subtree as already deleted.
    if (node == NULL) {
        return;
    }

    // Recursively deletes every occupied child slot.
    if (node->child_count > 0U) {
        size_t n = 0U;
        while(n != node->child_count) {
            recursively_delete(node->children[n]);
            n++;
        }
    }

    // Frees the sparse child-pointer array before the node itself.
    free(node->children);
    free(node);
    return;
}

// Frees every allocation owned by the trie.
void prefix_trie_destroy(PrefixTrie *trie) {
    // Treats a missing trie as a no-op.
    if (trie == NULL) {
        return;
    }

    // Deletes the full subtree rooted at the empty-prefix node.
    recursively_delete(trie->root);
    free(trie);

    return;
}

// Adds a missing suffix as a detached subtree, then links it into node.
static bool recursively_add(Node *node, const char *key) {
    // Rejects a missing destination node.
    if (node == NULL) {
        return false;
    }

    // Marks the current node as a complete key after every character is consumed.
    if (*key == '\0') {
        node->is_end_of_word = true;
        return true;
    }

    // Creates the first child node when no child pointers are occupied.
    if (node->child_count == 0U) {
        Node *new_node = malloc(sizeof(Node));
        if (new_node == NULL) {
            return false;
        }

        // Initializes the new detached node for the current key character.
        new_node->character = *key;
        new_node->child_capacity = 0U;
        new_node->child_count = 0U;
        key++;
        new_node->is_end_of_word = *key == '\0';
        new_node->children = NULL;
        // Builds the remaining suffix before mutating the live trie.
        if (!recursively_add(new_node, key)) {
            recursively_delete(new_node);
            return false;
        }

        // Allocates initial child-pointer capacity only when none exists.
        if (node->children == NULL) {
            Node **children = malloc(sizeof(*node->children) * 2);
            if (children == NULL) {
                recursively_delete(new_node);
                return false;
            }

            node->children = children;
            node->child_capacity = 2U;
        }

        // Links the fully built detached suffix as this node's first child.
        node->children[0] = new_node;
        node->child_count++;
        return true;
    } else {
        // Searches the existing sparse child collection for this character.
        size_t n = 0U;
        while(n != node->child_count) {
            if (node->children[n]->character == *key) {
                // Follows the existing child and consumes the current character.
                key++;
                if (!recursively_add(node->children[n], key)) {
                    return false;
                }

                return true;
            }

            n++;
        }

        // Builds a detached suffix for a character absent from this node.
        Node *new_node = malloc(sizeof(Node));
        if (new_node == NULL) {
            return false;
        }

        new_node->character = *key;
        new_node->child_capacity = 0U;
        new_node->child_count = 0U;
        key++;
        new_node->is_end_of_word = *key == '\0';
        new_node->children = NULL;

        if (!recursively_add(new_node, key)) {
            recursively_delete(new_node);
            return false;
        }

        // Grows the child-pointer array only when every slot is occupied.
        if (node->child_count == node->child_capacity) {
            if (node->child_capacity > SIZE_MAX / 2U) {
                recursively_delete(new_node);
                return false;
            }

            size_t new_capacity = node->child_capacity * 2;
            if (new_capacity > SIZE_MAX / sizeof(*node->children)) {
                recursively_delete(new_node);
                return false;
            }

            Node **children = realloc(node->children, new_capacity * sizeof(*node->children));
            if (children == NULL) {
                recursively_delete(new_node);
                return false;
            }

            node->children = children;
            node->child_capacity = new_capacity;
        }

        // Links the fully built suffix into the first free child slot.
        node->children[node->child_count] = new_node;
        node->child_count++;
        return true;
    }
}

// Inserts a complete key without double-counting duplicate insertion.
bool prefix_trie_insert(PrefixTrie *trie, const char *key) {
    // Rejects a missing trie or C string.
    if (trie == NULL) {
        return false;
    }

    // Rejects a missing C string.
    if (key == NULL) {
        return false;
    }

    // Empty strings are valid prefixes but not valid stored keys in this API.
    if (*key == '\0') {
        return false;
    }

    // Treats a previously stored key as an idempotent success.
    if (prefix_trie_contains(trie, key)) {
        return true;
    }

    // Adds the missing suffix, leaving the trie unchanged on allocation failure.
    if (!recursively_add(trie->root, key)) {
        return false;
    }

    // Counts the successfully inserted complete key.
    trie->size++;
    return true;
}

bool prefix_trie_contains(const PrefixTrie *trie, const char *key) {
    // Rejects a missing trie.
    if (trie == NULL) {
        return false;
    }

    // Rejects a missing C string.
    if (key == NULL) {
        return false;
    }

    // Starts at the empty-prefix root node.
    const Node *node = trie->root;
    // Points at the next character whose child path must be found.
    const char *temp = key;
    // Walks one trie edge for every character before the string terminator.
    while(*temp != '\0') {
        // Starts scanning this node's sparse collection of actual children.
        size_t n = 0U;
        // Tracks whether a child matches the current key character.
        bool found = false;
        while (n != node->child_count) {
            // Compares the child character against the character temp points at.
            if (node->children[n]->character == *temp) {
                // Advances to the next character only after consuming this edge.
                temp++;
                // Follows the matching child into its prefix node.
                node = node->children[n];
                // Records that this character path exists.
                found = true;
                break;
            }

            // Tries the next existing child when this one does not match.
            n++;
        }

        // A missing edge proves the complete key cannot be stored.
        if (!found) {
            return false;
        }
    }

    // A complete key must end on a node marked as a stored word.
    return node->is_end_of_word;
}

bool prefix_trie_starts_with(const PrefixTrie *trie, const char *prefix) {
    // Rejects a missing trie.
    if (trie == NULL) {
        return false;
    }

    // Rejects a missing C string.
    if (prefix == NULL) {
        return false;
    }

    // Starts at the empty-prefix root node.
    const Node *node = trie->root;
    // Points at the next prefix character whose child path must be found.
    const char *temp = prefix;
    // Walks one trie edge for every character before the string terminator.
    while(*temp != '\0') {
        // Starts scanning this node's sparse collection of actual children.
        size_t n = 0U;
        // Tracks whether a child matches the current prefix character.
        bool found = false;
        while(n != node->child_count) {
            // Follows the child whose stored character matches the prefix.
            if (node->children[n]->character == *temp) {
                node = node->children[n];
                temp++;
                found = true;
                break;
            }

            // Tries the next existing child when this one does not match.
            n++;
        }

        // A missing edge proves no stored key has this prefix.
        if (!found) {
            return false;
        }
    }

    // Reaching the end of any existing path is enough for a prefix query.
    return true;
}

// Removes one nonempty stored key and prunes unused suffix nodes on unwind.
static bool recursively_remove(Node *node, const char *key) {
    // Rejects a missing subtree.
    if (node == NULL) {
        return false;
    }

    // The public function rejects empty keys, and a parent handles the final
    // character directly before recursion receives the terminator.
    if (*key == '\0') {
        return false;
    }

    // Searches this node's sparse child collection for the next key character.
    size_t n = 0U;
    while(n != node->child_count) {
        if (node->children[n]->character == *key) {
            // Advances to the next key character after matching this child.
            key++;

            // Handles the final character without recursing into a terminator.
            if (*key == '\0') {
                // Rejects a path that is only a prefix, not a stored word.
                if (node->children[n]->is_end_of_word) {
                    // Removes the stored-word marker while preserving descendants.
                    node->children[n]->is_end_of_word = false;

                    // Deletes the child only when no longer needed by any word.
                    if (node->children[n]->child_count == 0) {
                        // Compacts the sparse array with its final occupied slot.
                        recursively_delete(node->children[n]);
                        node->children[n] = node->children[node->child_count - 1U];
                        node->children[node->child_count - 1U] = NULL;
                        node->child_count--;
                    }
                    
                    // Reports that the complete word was removed.
                    return true;
                }
            // Recurses through longer keys after consuming this character.
            } else if (recursively_remove(node->children[n], key)) {
                // Deletes a child that became an unused non-word leaf.
                if (node->children[n]->child_count == 0U && !node->children[n]->is_end_of_word) {
                    // Compacts the sparse array with its final occupied slot.
                    recursively_delete(node->children[n]);
                    node->children[n] = node->children[node->child_count - 1U];
                    node->children[node->child_count - 1U] = NULL;
                    node->child_count--;
                }

                // Propagates successful word removal even when this child remains.
                return true;
            }
        }

        // Tries the next existing child when this one does not match.
        n++;
    }

    // No child path matches the requested key.
    return false;
}

// Removes a nonempty stored key and decrements the stored-word count on success.
bool prefix_trie_remove(PrefixTrie *trie, const char *key) {
    // Rejects a missing trie or C string.
    if (trie == NULL) {
        return false;
    }

    if (key == NULL) {
        return false;
    }

    // Empty strings are valid prefixes but not valid stored keys in this API.
    if (*key == '\0') {
        return false;
    }

    // Rejects removal from an empty trie without traversal.
    if (trie->size == 0U) {
        return false;
    }

    // Removes the key only when it exists as a complete stored word.
    if (recursively_remove(trie->root, key)) {
        // Counts the successfully removed word.
        trie->size--;
        return true;
    } else {
        return false;
    }
}

size_t prefix_trie_size(const PrefixTrie *trie) {
    // Treats a missing trie as containing zero stored keys.
    if (trie == NULL) {
        return 0U;
    }

    // Returns the tracked number of complete stored keys.
    return trie->size;
}
