//
// Created by ilanit on 7/11/24.
//

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

// Remove and return a newsItem from the unbounded queue
newsItem popUnbounded(unboundedQueue *queue) {
    sem_wait(&queue->full); // Wait for the queue to have items
    pthread_mutex_lock(&queue->mutex); // Lock the mutex for thread safety

    if (queue->head == NULL) {
        // If the queue is empty, unlock the mutex and return a default newsItem
        pthread_mutex_unlock(&queue->mutex);
        return createNewsItem(-1); //handle empty queue case appropriately
    }
    Node *nodeToRemove = queue->head; // Get the node at the front of the queue
    newsItem item = nodeToRemove->item; // Extract the newsItem from the node
    queue->head = queue->head->next;  // Move the head pointer to the next node

    if (queue->head == NULL) {
        queue->tail = NULL; // If the queue is now empty, set the tail to NULL
    }
    queue->count--; // Decrement the count of items in the queue
    free(nodeToRemove);  // Free the memory of the removed node
    pthread_mutex_unlock(&queue->mutex); // Unlock the mutex

    return item;
}
