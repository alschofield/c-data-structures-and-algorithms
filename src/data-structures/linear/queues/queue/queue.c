// Provides the public Queue type and function declarations.
#include "queue.h"
// Declares malloc and free.
#include <stdlib.h>

// Defines the fields hidden from callers of the public API.
struct Queue {
    // Index of the next item to dequeue.
    size_t head;
    // Index where the next enqueued item will be stored.
    size_t tail;
    // Number of item pointers currently stored.
    size_t size;
    // Number of item-pointer slots currently allocated.
    size_t capacity;
    // Points to the dynamically allocated circular item buffer.
    void **items;
};

// Allocates and initializes an empty queue.
Queue *queue_create(void) {
    // Allocates memory for the Queue structure.
    Queue *queue = malloc(sizeof(Queue));
    // Checks whether allocation failed.
    if (queue == NULL) {
        // Reports allocation failure to the caller.
        return NULL;
    }

    // Starts the dequeue position at the beginning of the future buffer.
    queue->head = 0U;
    // Starts the enqueue position at the beginning of the future buffer.
    queue->tail = 0U;
    // Initializes the logical item count as zero.
    queue->size = 0U;
    // Initializes capacity because no item slots are allocated yet.
    queue->capacity = 0U;
    // Initializes the item-buffer pointer as a null pointer.
    queue->items = NULL;
    // Returns the initialized queue.
    return queue;
}

// Frees the allocations owned by the queue.
void queue_destroy(Queue *queue) {
    // Makes destruction of a null queue a safe no-op.
    if (queue == NULL) {
        // Returns because there is no allocation to release.
        return;
    }

    // Frees the circular item-pointer buffer.
    free(queue->items);
    // Frees the Queue structure.
    free(queue);
    // Ends the void function.
    return;
}

// Adds an item pointer to the back of the queue.
bool queue_enqueue(Queue *queue, void *item) {
    // Rejects a missing queue pointer.
    if (queue == NULL) {
        // Reports failure because there is no queue to modify.
        return false;
    }

    // Grows the item buffer only when every allocated slot is occupied.
    if (queue->size == queue->capacity) {
        // Uses two slots for the first allocation and doubles later allocations.
        size_t new_capacity = queue->capacity == 0U ? 2U : queue->capacity * 2U;
        // Allocates a new buffer so wrapped items can be copied into FIFO order.
        void **temp = malloc(new_capacity * sizeof(*queue->items));
        // Checks whether allocation failed.
        if (temp == NULL) {
            // Leaves the old buffer and queue state unchanged.
            return false;
        }

        // Copies each logical queue item from the old circular buffer.
        for (size_t i = 0U; i < queue->size; i++) {
            // Writes the ith FIFO item into its new contiguous position.
            temp[i] = queue->items[(queue->head + i) % queue->capacity];
        }

        // Releases the old circular buffer after every item was copied.
        free(queue->items);
        // Stores the address of the new item buffer.
        queue->items = temp;
        // Resets the first logical item to the first physical slot.
        queue->head = 0U;
        // Sets the next free slot immediately after the copied items.
        queue->tail = queue->size;
        // Records the new number of available slots.
        queue->capacity = new_capacity;
    }

    // Stores the new pointer in the current back slot.
    queue->items[queue->tail] = item;
    // Increases the logical item count.
    queue->size++;
    // Advances the back slot, wrapping to zero at the end of the buffer.
    queue->tail = (queue->tail + 1U) % queue->capacity;
    // Reports successful insertion.
    return true;
}

// Removes the front item and returns it through out_item.
bool queue_dequeue(Queue *queue, void **out_item) {
    // Rejects a missing queue pointer.
    if (queue == NULL) {
        // Reports failure because there is no queue to modify.
        return false;
    }

    // Rejects removal from an empty queue.
    if (queue->size == 0U) {
        // Reports that no item is available.
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        // Reports failure without removing an item.
        return false;
    }

    // Copies the front pointer into the caller's output variable.
    *out_item = queue->items[queue->head];
    // Clears the now-unused pointer slot.
    queue->items[queue->head] = NULL;
    // Advances the front slot, wrapping to zero at the end of the buffer.
    queue->head = (queue->head + 1U) % queue->capacity;
    // Decreases the logical item count.
    queue->size--;
    // Reports successful removal.
    return true;
}

// Copies the front item pointer without removing it.
bool queue_peek(const Queue *queue, void **out_item) {
    // Rejects a missing queue pointer.
    if (queue == NULL) {
        // Reports failure because there is no queue to inspect.
        return false;
    }

    // Rejects inspection of an empty queue.
    if (queue->size == 0U) {
        // Reports that no front item exists.
        return false;
    }

    // Rejects a missing output location.
    if (out_item == NULL) {
        // Reports failure without changing queue state.
        return false;
    }

    // Copies the front pointer into the caller's output variable.
    *out_item = queue->items[queue->head];
    // Reports successful inspection without changing size.
    return true;
}

// Returns the number of item pointers stored.
size_t queue_size(const Queue *queue) {
    // Treats a null queue as containing zero items.
    if (queue == NULL) {
        // Returns the empty-queue count.
        return 0U;
    }

    // Returns the current logical item count.
    return queue->size;
}

// Reports whether a queue contains no item pointers.
bool queue_is_empty(const Queue *queue) {
    // Treats a null queue as empty under this API contract.
    if (queue == NULL) {
        // Reports an empty queue.
        return true;
    }

    // Compares the logical item count with zero.
    return queue->size == 0U;
}
