// Provides the public DoublyLinkedList type and function declarations.
#include "doubly_linked_list.h"

// Declares malloc and free.
#include <stdlib.h>

// Defines one node in the bidirectional chain.
struct Node {
    // Stores the caller-owned item pointer.
    void *value;
    // Points to the next node, or NULL for the final node.
    Node *next;
    // Points to the previous node, or NULL for the first node.
    Node *prev;
};
// Defines the fields hidden from callers of the public API.
struct DoublyLinkedList {
    // Counts the nodes currently linked into the list.
    size_t size;
    // Points to the first node, or NULL when the list is empty.
    Node *first;
    // Points to the last node, or NULL when the list is empty.
    Node *last;
};

// Allocates and initializes an empty doubly linked list.
DoublyLinkedList *doubly_linked_list_create(void) {
    // Allocates memory for the list structure.
    DoublyLinkedList *list = malloc(sizeof(DoublyLinkedList));

    // Checks whether allocation failed.
    if (list == NULL) {
        // Reports allocation failure to the caller.
        return NULL;
    }

    // Initializes the logical node count as zero.
    list->size = 0U;
    // Initializes the first pointer because no nodes exist yet.
    list->first = NULL;
    // Initializes the last pointer because no nodes exist yet.
    list->last = NULL;

    // Returns the initialized list.
    return list;
}

// Frees every node and then frees the list structure.
void doubly_linked_list_destroy(DoublyLinkedList *list) {
    // Treats a NULL list as a no-op.
    if (list == NULL) {
        return;
    }

    // delete each node
    // Starts the walk at the first node.
    Node *temp = list->first;
    // Holds the successor so it survives freeing the current node.
    Node *next = NULL;
    // Walks the chain until every node is freed.
    while(temp != NULL) {
        // Saves the successor before the current node is freed.
        next = temp->next;
        // Frees the current node without touching the caller-owned value.
        free(temp);
        // Advances to the saved successor.
        temp = next;
    }

    // Frees the list structure itself.
    free(list);
    return;
}

// Inserts an item as the new first node.
bool doubly_linked_list_push_front(DoublyLinkedList *list, void *item) {
    // Rejects a NULL list.
    if (list == NULL) {
        return false;
    }

    // Allocates the node that will hold the item.
    Node *new = malloc(sizeof(Node));
    // Checks whether allocation failed.
    if (new == NULL) {
        // Reports allocation failure without modifying the list.
        return false;
    }
    
    // Stores the caller-owned item pointer.
    new->value = item;
    // Checks whether an existing first node needs its back-link updated.
    if (list->first != NULL) {
        // Points the old first node back at the new node.
        list->first->prev = new;
    }

    // Links the new node ahead of the old first node (or NULL when empty).
    new->next = list->first;
    // Marks the new node as the first by clearing its back-link.
    new->prev = NULL;
    // Publishes the new node as the first.
    list->first = new;

    // Checks whether the list was empty before this push.
    if(list->last == NULL) {
        // Makes the sole node both first and last.
        list->last = new;
    }

    // Records the added node in the size.
    list->size++;

    // Reports a successful push.
    return true;
}

// Inserts an item as the new last node.
bool doubly_linked_list_push_back(DoublyLinkedList *list, void *item) {
    // Rejects a NULL list.
    if (list == NULL) {
        return false;
    }

    // Allocates the node that will hold the item.
    Node *new = malloc(sizeof(Node));
    // Checks whether allocation failed.
    if (new == NULL) {
        // Reports allocation failure without modifying the list.
        return false;
    }

    // Stores the caller-owned item pointer.
    new->value = item;
    // Checks whether an existing last node needs its forward link updated.
    if (list->last != NULL) {
        // Points the old last node forward at the new node.
        list->last->next = new;
    }

    // Marks the new node as the last by clearing its forward link.
    new->next = NULL;
    // Links the new node back at the old last node (or NULL when empty).
    new->prev = list->last;
    // Publishes the new node as the last.
    list->last = new;

    // Checks whether the list was empty before this push.
    if (list->first == NULL) {
        // Makes the sole node both first and last.
        list->first = new;
    }

    // Records the added node in the size.
    list->size++;

    // Reports a successful push.
    return true;
}

// Removes the first node and hands its value to the caller.
bool doubly_linked_list_pop_front(DoublyLinkedList *list, void **out_item) {
    // Rejects a NULL list.
    if (list == NULL) {
        return false;
    }

    // Rejects a NULL output parameter.
    if (out_item == NULL) {
        return false;
    }

    // Rejects an empty list.
    if (list->size == 0U) {
        return false;
    }

    // Keeps the departing first node reachable for the free below.
    Node *temp = list->first;
    // Hands the stored value to the caller.
    *out_item = temp->value;
    // Promotes the second node to first.
    list->first = temp->next;

    // Checks whether the pop emptied the list.
    if (list->first == NULL) {
        // Clears the last pointer so both ends agree the list is empty.
        list->last = NULL;
    } else {
        // Marks the promoted node as first by clearing its back-link.
        list->first->prev = NULL;
    }

    // Records the removed node in the size.
    list->size--;

    // Frees the unlinked node without touching the caller-owned value.
    free(temp);

    // Reports a successful pop.
    return true;
}

// Removes the last node and hands its value to the caller.
bool doubly_linked_list_pop_back(DoublyLinkedList *list, void **out_item) {
    // Rejects a NULL list.
    if (list == NULL) {
        return false;
    }

    // Rejects a NULL output parameter.
    if (out_item == NULL) {
        return false;
    }

    // Rejects an empty list.
    if (list->size == 0U) {
        return false;
    }

    // Keeps the departing last node reachable for the free below.
    Node *temp = list->last;
    // Hands the stored value to the caller.
    *out_item = temp->value;
    // Demotes the second-to-last node to last.
    list->last = temp->prev;
    
    // Checks whether the pop emptied the list.
    if (list->last == NULL) {
        // Clears the first pointer so both ends agree the list is empty.
        list->first = NULL;
    } else {
        // Marks the demoted node as last by clearing its forward link.
        list->last->next = NULL;
    }

    // Records the removed node in the size.
    list->size--;
    
    // Frees the unlinked node without touching the caller-owned value.
    free(temp);

    // Reports a successful pop.
    return true;
}

// Reads the value at an index without modifying the list.
bool doubly_linked_list_get(const DoublyLinkedList *list, size_t index, void **out_item) {
    // Rejects a NULL list.
    if (list == NULL) {
        return false;
    }

    // Rejects a NULL output parameter.
    if (out_item == NULL) {
        return false;
    }

    // Rejects an empty list.
    if (list->size == 0U) {
        return false;
    }

    // Rejects out-of-range indexes; valid element indexes are [0, size).
    if (index >= list->size) {
        return false;
    }

    // Chooses the nearer end: 0 walks forward from first, 1 backward from last.
    size_t direction = index < (list->size / 2) ? 0U : 1U;
    // Starts the position counter at the chosen end's index.
    size_t n = direction == 0U ? 0U : (list->size - 1U);
    // Starts the walk at the chosen end's node.
    Node *temp = direction == 0U ? list->first : list->last;
    // Walks until the counter reaches the requested index.
    while(n != index) {
        // Checks which direction this walk is moving.
        if (direction == 0U) {
            // Steps one node toward the tail.
            temp = temp->next;
            // Advances the position counter to match.
            n++;
        } else {
            // Steps one node toward the head.
            temp = temp->prev;
            // Retreats the position counter to match.
            n--;
        }
    }

    // Hands the stored value to the caller.
    *out_item = temp->value;

    // Reports a successful read.
    return true;
}

// Inserts an item so it occupies the given index.
bool doubly_linked_list_insert(DoublyLinkedList *list, size_t index, void *item) {
    // Rejects a NULL list.
    if (list == NULL) {
        return false;
    }

    // Rejects indexes beyond the append position.
    if (index > list->size) {
        return false;
    }

    // Delegates a front insert so first/last syncing lives in one place.
    if (index == 0U) {
        return doubly_linked_list_push_front(list, item);
    }

    // Delegates an append so first/last syncing lives in one place.
    if (index == list->size) {
        return doubly_linked_list_push_back(list, item);
    }

    // Allocates the node that will hold the item.
    Node *new = malloc(sizeof(Node));
    // Checks whether allocation failed.
    if (new == NULL) {
        // Reports allocation failure without modifying the list.
        return false;
    }

    // Stores the caller-owned item pointer.
    new->value = item;

    // Chooses the nearer end: 0 walks forward from first, 1 backward from last.
    size_t direction = index < (list->size / 2) ? 0U : 1U;
    // Starts the position counter at the chosen end's index.
    size_t n = direction == 0U ? 0U : (list->size - 1U);
    // Starts the walk at the chosen end's node.
    Node *temp = direction == 0U ? list->first : list->last;

    // Walks until the counter reaches the requested index.
    while(n != index) {
        // Checks which direction this walk is moving.
        if (direction == 0U) {
            // Steps one node toward the tail.
            temp = temp->next;
            // Advances the position counter to match.
            n++;
        } else {
            // Steps one node toward the head.
            temp = temp->prev;
            // Retreats the position counter to match.
            n--;
        }
    }

    // Splices the new node in front of temp so it takes over temp's index;
    // temp->prev is always valid because index 0 was delegated above.
    // Routes the predecessor forward to the new node.
    temp->prev->next = new;
    // Links the new node back at the predecessor.
    new->prev = temp->prev;
    // Links the new node forward at the displaced node.
    new->next = temp;
    // Routes the displaced node back to the new node.
    temp->prev = new;
    // Records the added node in the size.
    list->size++;
    // Reports a successful insert.
    return true;
}

// Removes the node at an index and hands its value to the caller.
bool doubly_linked_list_remove(DoublyLinkedList *list, size_t index, void **out_item) {
    // Rejects a NULL list.
    if (list == NULL) {
        return false;
    }

    // Rejects a NULL output parameter.
    if (out_item == NULL) {
        return false;
    }

    // Rejects out-of-range indexes, including every index on an empty list.
    if (index >= list->size) {
        return false;
    }

    // Never taken: the >= check above already rejects everything when empty.
    if (list->size == 0U) {
        return false;
    }

    // Delegates a front removal so first/last syncing lives in one place.
    if (index == 0U) {
        return doubly_linked_list_pop_front(list, out_item);
    }

    // Delegates a back removal so first/last syncing lives in one place.
    if (index == list->size - 1) {
        return doubly_linked_list_pop_back(list, out_item);
    }

    // Chooses the nearer end: 0 walks forward from first, 1 backward from last.
    size_t direction = index < (list->size / 2) ? 0U : 1U;
    // Starts the position counter at the chosen end's index.
    size_t n = direction == 0U ? 0U : (list->size - 1U);
    // Starts the walk at the chosen end's node.
    Node *temp = direction == 0U ? list->first : list->last;

    // Walks until the counter reaches the requested index.
    while(n != index) {
        // Checks which direction this walk is moving.
        if (direction == 0U) {
            // Steps one node toward the tail.
            temp = temp->next;
            // Advances the position counter to match.
            n++;
        } else {
            // Steps one node toward the head.
            temp = temp->prev;
            // Retreats the position counter to match.
            n--;
        }
    }

    // Routes the predecessor around the departing node; both neighbors exist
    // because the end cases were delegated above.
    temp->prev->next = temp->next;
    // Routes the successor back around the departing node.
    temp->next->prev = temp->prev;

    // Hands the stored value to the caller.
    *out_item = temp->value;

    // Frees the unlinked node without touching the caller-owned value.
    free(temp);

    // Records the removed node in the size.
    list->size--;
    // Reports a successful removal.
    return true;
}

// Reports how many nodes the list holds.
size_t doubly_linked_list_size(const DoublyLinkedList *list) {
    // Treats a NULL list as holding zero nodes.
    if (list == NULL) {
        return 0U;
    }

    // Returns the tracked node count.
    return list->size;
}

// Reports whether the list holds no nodes.
bool doubly_linked_list_is_empty(const DoublyLinkedList *list) {
    // Treats a NULL list as empty, matching the singly linked list.
    if (list == NULL) {
        return true;
    }

    // Compares the tracked node count against zero.
    return list->size == 0U;
}
