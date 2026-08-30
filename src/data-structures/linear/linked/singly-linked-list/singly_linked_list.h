// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Names the opaque singly linked list.
typedef struct SinglyLinkedList SinglyLinkedList;
// Names the private list-node type used by implementation helpers.
typedef struct Node Node;

// Allocates an empty singly linked list.
SinglyLinkedList *singly_linked_list_create(void);
// Frees list nodes without freeing caller-owned items.
void singly_linked_list_destroy(SinglyLinkedList *list);
// Adds one item at the list head.
bool singly_linked_list_push_front(SinglyLinkedList *list, void *item);
// Adds one item at the list tail.
bool singly_linked_list_push_back(SinglyLinkedList *list, void *item);
// Removes and returns the head item.
bool singly_linked_list_pop_front(SinglyLinkedList *list, void **out_item);
// Removes and returns the tail item.
bool singly_linked_list_pop_back(SinglyLinkedList *list, void **out_item);
// Returns one item by zero-based index.
bool singly_linked_list_get(const SinglyLinkedList *list, size_t index, void **out_item);
// Inserts one item at index.
bool singly_linked_list_insert(SinglyLinkedList *list, size_t index, void *item);
// Removes and returns one item at index.
bool singly_linked_list_remove(SinglyLinkedList *list, size_t index, void **out_item);
// Returns the number of stored items.
size_t singly_linked_list_size(const SinglyLinkedList *list);
// Reports whether the list contains no items.
bool singly_linked_list_is_empty(const SinglyLinkedList *list);
