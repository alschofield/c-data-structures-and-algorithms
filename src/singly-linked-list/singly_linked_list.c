// Provides the public SinglyLinkedList type and function declarations.
#include "singly_linked_list.h"
// Declares malloc and free.
#include <stdlib.h>

// Defines the fields hidden from callers of the public API.
struct SinglyLinkedList {
    // Counts the nodes currently linked into the list.
    size_t size;
    // Points to the first node, or NULL when the list is empty.
    Node *head;
};

// Defines one node in the forward-only chain.
struct Node {
    // Points to the next node, or NULL for the final node.
    Node *next;
    // Stores the caller-owned item pointer.
    void *value;
};

// Allocates and initializes an empty singly linked list.
SinglyLinkedList *singly_linked_list_create(void) {
    // Allocates memory for the list structure.
    SinglyLinkedList *list = malloc(sizeof(SinglyLinkedList));
    // Checks whether allocation failed.
    if (list == NULL) {
        // Reports allocation failure to the caller.
        return NULL;
    }

    // Initializes the logical node count as zero.
    list->size = 0U;
    // Initializes the head because no nodes exist yet.
    list->head = NULL;
    // Returns the initialized list.
    return list;
}

// Frees every node and then frees the list structure.
void singly_linked_list_destroy(SinglyLinkedList *list) {
    // Makes destruction of a null list a safe no-op.
    if (list == NULL) {
        // Returns because there is no allocation to release.
        return;
    }

    // Starts traversal at the first node.
    Node *temp = list->head;
    // Holds the next node before the current node is freed.
    Node *next = list->head;
    // Continues until every node has been released.
    while (temp != NULL) {
        // Saves the forward link before freeing its owning node.
        next = temp->next;
        // Frees the node but never frees its caller-owned value.
        free(temp);
        // Advances to the node saved before the free.
        temp = next;
    }

    // Frees the list wrapper after all nodes are released.
    free(list);
    // Ends the void function.
    return;
}

// Adds an item pointer at the front of the list.
bool singly_linked_list_push_front(SinglyLinkedList *list, void *item) {
    // Rejects a missing list pointer.
    if (list == NULL) {
        // Reports failure because there is no list to modify.
        return false;
    }

    // Allocates one node for the new item pointer.
    Node *node = malloc(sizeof(Node));
    // Checks whether allocation failed.
    if (node == NULL) {
        // Leaves the list unchanged and reports failure.
        return false;
    }

    // Stores the caller-owned item pointer in the new node.
    node->value = item;
    // Links the new node to the current first node.
    node->next = list->head;
    // Makes the new node the first node in the list.
    list->head = node;
    // Increases the logical node count.
    list->size++;
    // Reports successful insertion.
    return true;
}

// Adds an item pointer at the back of the list.
bool singly_linked_list_push_back(SinglyLinkedList *list, void *item) {
    // Rejects a missing list pointer.
    if (list == NULL) {
        // Reports failure because there is no list to modify.
        return false;
    }

    // Allocates one node for the new item pointer.
    Node *node = malloc(sizeof(Node));
    // Checks whether allocation failed.
    if (node == NULL) {
        // Leaves the list unchanged and reports failure.
        return false;
    }

    // Stores the caller-owned item pointer in the new node.
    node->value = item;
    // Marks this new node as the final node.
    node->next = NULL;

    // Handles insertion into an empty list.
    if (list->size == 0U) {
        // Makes the new node the first node.
        list->head = node;
        // Increases the logical node count.
        list->size++;
        // Reports successful insertion.
        return true;
    }

    // Starts traversal at the first existing node.
    Node *temp = list->head;
    // Advances until temp points at the final existing node.
    for (size_t i = 0U; i < list->size - 1U; i++) {
        // Follows the next link by one node.
        temp = temp->next;
    }

    // Links the final existing node to the new final node.
    temp->next = node;
    // Increases the logical node count.
    list->size++;
    // Reports successful insertion.
    return true;
}

// Removes the front item and returns it through out_item.
bool singly_linked_list_pop_front(SinglyLinkedList *list, void **out_item) {
    // Rejects a missing list pointer.
    if (list == NULL) {
        // Reports failure because there is no list to modify.
        return false;
    }

    // Rejects removal from an empty list.
    if (list->size == 0U) {
        // Reports that no item is available.
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        // Reports failure without removing an item.
        return false;
    }

    // Saves the second node before freeing the current head.
    Node *next_head = list->head->next;
    // Copies the removed item pointer into the caller's output variable.
    *out_item = list->head->value;
    // Frees the removed node but not its caller-owned value.
    free(list->head);
    // Makes the saved second node the new head.
    list->head = next_head;
    // Decreases the logical node count.
    list->size--;
    // Reports successful removal.
    return true;
}

// Removes the back item and returns it through out_item.
bool singly_linked_list_pop_back(SinglyLinkedList *list, void **out_item) {
    // Rejects a missing list pointer.
    if (list == NULL) {
        // Reports failure because there is no list to modify.
        return false;
    }

    // Rejects removal from an empty list.
    if (list->size == 0U) {
        // Reports that no item is available.
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        // Reports failure without removing an item.
        return false;
    }

    // Handles the one-node case without traversing or dereferencing next.
    if (list->size == 1U) {
        // Copies the only item pointer into the caller's output variable.
        *out_item = list->head->value;
        // Frees the only node but not its caller-owned value.
        free(list->head);
        // Restores the empty-list head invariant.
        list->head = NULL;
        // Decreases the logical node count to zero.
        list->size--;
        // Reports successful removal.
        return true;
    }

    // Starts traversal at the first node.
    Node *temp = list->head;
    // Advances until temp points at the node before the final node.
    for (size_t i = 0U; i < list->size - 2U; i++) {
        // Follows the next link by one node.
        temp = temp->next;
    }

    // Copies the final item pointer into the caller's output variable.
    *out_item = temp->next->value;
    // Frees the final node but not its caller-owned value.
    free(temp->next);
    // Makes temp the new final node.
    temp->next = NULL;
    // Decreases the logical node count.
    list->size--;
    // Reports successful removal.
    return true;
}

// Copies an item pointer at an index without modifying the list.
bool singly_linked_list_get(const SinglyLinkedList *list, size_t index, void **out_item) {
    // Rejects a missing list pointer.
    if (list == NULL) {
        // Reports failure because there is no list to inspect.
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        // Reports failure without changing an output pointer.
        return false;
    }

    // Rejects lookup in an empty list.
    if (list->size == 0U) {
        // Reports that no item is available.
        return false;
    }

    // Rejects indexes outside the list's valid range.
    if (index >= list->size) {
        // Leaves the caller's output pointer unchanged.
        return false;
    }

    // Starts traversal at the first node without permitting mutation.
    const Node *temp = list->head;
    // Counts how many forward links have been followed.
    size_t n = 0U;
    // Stops when temp reaches the node at the requested index.
    while (n != index) {
        // Follows the next link by one node.
        temp = temp->next;
        // Records the completed forward traversal.
        n++;
    }

    // Copies the requested caller-owned item pointer to the output location.
    *out_item = temp->value;
    // Reports successful lookup without changing the list.
    return true;
}

// Inserts an item pointer before the node currently at index.
bool singly_linked_list_insert(SinglyLinkedList *list, size_t index, void *item) {
    // Rejects a missing list pointer.
    if (list == NULL) {
        // Reports failure because there is no list to modify.
        return false;
    }

    // Rejects indexes beyond the valid append position.
    if (list->size < index) {
        // Leaves the list unchanged and reports failure.
        return false;
    }

    // Delegates head insertion because index zero has no predecessor node.
    if (index == 0U) {
        // Preserves push_front's allocation and failure behavior.
        return singly_linked_list_push_front(list, item);
    }

    // Allocates one node for the new item pointer.
    Node *new_node = malloc(sizeof(Node));
    // Checks whether allocation failed.
    if (new_node == NULL) {
        // Leaves the list unchanged and reports failure.
        return false;
    }

    // Stores the caller-owned item pointer in the new node.
    new_node->value = item;
    // Starts traversal at the first node, which has index zero.
    Node *temp = list->head;
    // Counts how many forward links have been followed.
    size_t n = 0U;
    // Stops at the node immediately before the insertion position.
    while (n != index - 1U) {
        // Follows the next link by one node.
        temp = temp->next;
        // Records the completed forward traversal.
        n++;
    }

    // Links the new node to the old node at index, or NULL when appending.
    new_node->next = temp->next;
    // Links the predecessor to the new node.
    temp->next = new_node;
    // Increases the logical node count.
    list->size++;
    // Reports successful insertion.
    return true;
}

// Removes the item pointer at an index and returns it through out_item.
bool singly_linked_list_remove(SinglyLinkedList *list, size_t index, void **out_item) {
    // Rejects a missing list pointer.
    if (list == NULL) {
        // Reports failure because there is no list to modify.
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        // Reports failure without removing an item.
        return false;
    }

    // Rejects indexes outside the list's valid range.
    if (index >= list->size) {
        // Leaves the list and caller's output pointer unchanged.
        return false;
    }

    // Delegates head removal because index zero has no predecessor node.
    if (index == 0U) {
        // Preserves pop_front's final-node and output behavior.
        return singly_linked_list_pop_front(list, out_item);
    }

    // Starts traversal at the first node, which has index zero.
    Node *temp = list->head;
    // Counts how many forward links have been followed.
    size_t n = 0U;
    // Stops at the node immediately before the removal position.
    while (n != index - 1U) {
        // Follows the next link by one node.
        temp = temp->next;
        // Records the completed forward traversal.
        n++;
    }

    // Saves the successor before the removed node is freed.
    Node *next = temp->next->next;
    // Copies the removed caller-owned item pointer into the output location.
    *out_item = temp->next->value;
    // Frees the removed node but not its caller-owned value.
    free(temp->next);
    // Bypasses the removed node; next is NULL when removing the tail.
    temp->next = next;
    // Decreases the logical node count.
    list->size--;
    // Reports successful removal.
    return true;
}

// Returns the number of nodes stored.
size_t singly_linked_list_size(const SinglyLinkedList *list) {
    // Treats a null list as containing zero nodes.
    if (list == NULL) {
        // Returns the empty-list count.
        return 0U;
    }

    // Returns the current logical node count.
    return list->size;
}

// Reports whether a list contains no nodes.
bool singly_linked_list_is_empty(const SinglyLinkedList *list) {
    // Treats a null list as empty under this API contract.
    if (list == NULL) {
        // Reports an empty list.
        return true;
    }

    // Compares the logical node count with zero.
    return list->size == 0U;
}
