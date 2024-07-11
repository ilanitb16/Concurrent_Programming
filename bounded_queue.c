//
// Created by ilanit on 7/11/24.
//

#include "bounded_queue.h"
#include <stdlib.h>
#include <string.h>

// Create and initialize a bounded queue
boundedQueue *createBoundedQueue(int id, int size, int numOfProducts) {
    boundedQueue *queue = malloc(sizeof(boundedQueue)); // Allocate memory for the queue
    queue->id = id; // Set the queue ID
    queue->capacity = size; // Set the maximum capacity of the queue
    queue->numOfProducts = numOfProducts; // Set the number of products the producer will produce
    queue->front = 0; // Initialize the front index to 0
    queue->rear = 0; // Initialize the rear index to 0
    queue->count = 0; // Initialize the count of items in the queue to 0
    queue->finished = 0; // Initialize the finished flag to 0
    pthread_mutex_init(&queue->mutex, NULL); // Initialize the mutex
    sem_init(&queue->full, 0, 0); // Initialize the full semaphore to 0
    sem_init(&queue->empty, 0, size); // Initialize the empty semaphore to the queue's capacity
    queue->buffer = malloc(sizeof(newsItem) * (size + 1)); // Allocate memory for the buffer
    queue->types[0] = 0; // Initialize the count of sports items to 0
    queue->types[1] = 0; // Initialize the count of weather items to 0
    queue->types[2] = 0; // Initialize the count of news items to 0
    return queue;
}

// Insert a news item into the bounded queue
void insertBounded(boundedQueue *queue, newsItem item) {
    sem_wait(&queue->empty); // Wait until there is space in the queue
    pthread_mutex_lock(&queue->mutex); // Lock the queue

    queue->buffer[queue->rear] = item; // Insert the news item into the queue
    queue->rear = (queue->rear + 1) % queue->capacity; // Update the rear index
    queue->count++; // Increment the count of items in the queue

    pthread_mutex_unlock(&queue->mutex); // Unlock the queue
    sem_post(&queue->full); // Signal that the queue has new items
}

// Remove and return a news item from the bounded queue
newsItem popBounded(boundedQueue *queue) {
    sem_wait(&queue->full); // Wait until there are items in the queue
    pthread_mutex_lock(&queue->mutex); // Lock the queue

    newsItem item = queue->buffer[queue->front]; // Remove the news item from the queue
    queue->front = (queue->front + 1) % queue->capacity; // Update the front index
    queue->count--; // Decrement the count of items in the queue

    pthread_mutex_unlock(&queue->mutex); // Unlock the queue
    sem_post(&queue->empty); // Signal that there is space in the queue

    return item;
}

