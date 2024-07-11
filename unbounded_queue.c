#include "unbounded_queue.h"
#include <stdlib.h>
#include <string.h>

// Helper function to create a newsItem with a specified id and random type
newsItem createNewsItem(int id) {
    newsItem item;
    item.id = id; // Set the id of the newsItem to the provided id
    item.type = (newsType) (rand() % 3); // Randomly assign one of the news types
    item.numOfType = 0; // Initialize the number of this type of newsItem to 0
    return item; // Return the created newsItem
}

// Initialize the unbounded queue with default values
void initUnbounded(unboundedQueue *queue) {
    queue->id = 0;
    queue->tail = NULL;
    queue->head = NULL;
    queue->count = 0;
    queue->finished = 0;
    pthread_mutex_init(&queue->mutex, NULL); //Initialize the mutex
    sem_init(&queue->full, 0, 0); // Initialize the semaphore
}

// Create and initialize a new unbounded queue with a specified id
unboundedQueue *createUnboundedQueue(int id) {
    unboundedQueue *queue = malloc(sizeof(unboundedQueue)); // Allocate memory for the queue
    queue->id = id;
    initUnbounded(queue); // Initialize the queue
    return queue;
}

// Insert a newsItem into the unbounded queue
void insertUnbounded(unboundedQueue *queue, newsItem item) {
    Node *newNode = malloc(sizeof(Node)); // Allocate memory for a new node
    newNode->item = item;
    newNode->next = NULL;

    pthread_mutex_lock(&queue->mutex); // Lock the mutex for thread safety
    if (queue->tail == NULL) {
        // If the queue is empty, set both head and tail to the new node
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        // Otherwise, add the new node to the end of the queue
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
    queue->count++; // Increment the count of items in the queue
    pthread_mutex_unlock(&queue->mutex);

    sem_post(&queue->full); // Signal that the queue has a new item
}

newsItem popUnbounded(unboundedQueue *queue) {
    sem_wait(&queue->full);
    pthread_mutex_lock(&queue->mutex);
    if (queue->head == NULL) {
        pthread_mutex_unlock(&queue->mutex);
        return createNewsItem(-1); //handle empty queue case appropriately
    }
    Node *nodeToRemove = queue->head;
    newsItem item = nodeToRemove->item;
    queue->head = queue->head->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    queue->count--;
    free(nodeToRemove);
    pthread_mutex_unlock(&queue->mutex);
    return item;
}
