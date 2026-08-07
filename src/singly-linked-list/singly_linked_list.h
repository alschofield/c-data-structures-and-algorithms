#include <stdbool.h>
#include <stddef.h>

typedef struct SinglyLinkedList SinglyLinkedList;
typedef struct Node Node;

SinglyLinkedList *singly_linked_list_create(void);
void singly_linked_list_destroy(SinglyLinkedList *list);
bool singly_linked_list_push_front(SinglyLinkedList *list, void *item);
bool singly_linked_list_push_back(SinglyLinkedList *list, void *item);
bool singly_linked_list_pop_front(SinglyLinkedList *list, void **out_item);
bool singly_linked_list_pop_back(SinglyLinkedList *list, void **out_item);
bool singly_linked_list_get(const SinglyLinkedList *list, size_t index, void **out_item);
bool singly_linked_list_insert(SinglyLinkedList *list, size_t index, void *item);
bool singly_linked_list_remove(SinglyLinkedList *list, size_t index, void **out_item);
size_t singly_linked_list_size(const SinglyLinkedList *list);
bool singly_linked_list_is_empty(const SinglyLinkedList *list);
