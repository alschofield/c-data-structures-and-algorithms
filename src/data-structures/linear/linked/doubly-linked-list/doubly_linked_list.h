// Declares bool.
#include <stdbool.h>
// Declares size_t.
#include <stddef.h>

// Names the private doubly linked-list node type.
typedef struct Node Node;
// Names the opaque doubly linked list.
typedef struct DoublyLinkedList DoublyLinkedList;

// Allocates an empty doubly linked list.
DoublyLinkedList *doubly_linked_list_create(void);
// Frees list nodes without freeing caller-owned items.
void doubly_linked_list_destroy(DoublyLinkedList *list);
// Adds one item at the list head.
bool doubly_linked_list_push_front(DoublyLinkedList *list, void *item);
// Adds one item at the list tail.
bool doubly_linked_list_push_back(DoublyLinkedList *list, void *item);
// Removes and returns the head item.
bool doubly_linked_list_pop_front(DoublyLinkedList *list, void **out_item);
// Removes and returns the tail item.
bool doubly_linked_list_pop_back(DoublyLinkedList *list, void **out_item);
// Returns one item by zero-based index.
bool doubly_linked_list_get(const DoublyLinkedList *list, size_t index, void **out_item);
// Inserts one item at index.
bool doubly_linked_list_insert(DoublyLinkedList *list, size_t index, void *item);
// Removes and returns one item at index.
bool doubly_linked_list_remove(DoublyLinkedList *list, size_t index, void **out_item);
// Returns the number of stored items.
size_t doubly_linked_list_size(const DoublyLinkedList *list);
// Reports whether the list contains no items.
bool doubly_linked_list_is_empty(const DoublyLinkedList *list);
