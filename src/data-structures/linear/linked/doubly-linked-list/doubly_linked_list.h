#include <stdbool.h>
#include <stddef.h>

typedef struct Node Node;
typedef struct DoublyLinkedList DoublyLinkedList;

DoublyLinkedList *doubly_linked_list_create(void);
void doubly_linked_list_destroy(DoublyLinkedList *list);
bool doubly_linked_list_push_front(DoublyLinkedList *list, void *item);
bool doubly_linked_list_push_back(DoublyLinkedList *list, void *item);
bool doubly_linked_list_pop_front(DoublyLinkedList *list, void **out_item);
bool doubly_linked_list_pop_back(DoublyLinkedList *list, void **out_item);
bool doubly_linked_list_get(const DoublyLinkedList *list, size_t index, void **out_item);
bool doubly_linked_list_insert(DoublyLinkedList *list, size_t index, void *item);
bool doubly_linked_list_remove(DoublyLinkedList *list, size_t index, void **out_item);
size_t doubly_linked_list_size(const DoublyLinkedList *list);
bool doubly_linked_list_is_empty(const DoublyLinkedList *list);
