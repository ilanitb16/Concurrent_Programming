//
// Created by oem on 7/11/24.
//

//
// Created by oem on 7/11/24.
//

#include "bounded_queue.h"
#include <stdlib.h>
#include <string.h>

boundedQueue *createBoundedQueue(int id, int size, int numOfProducts) {
    boundedQueue *queue = malloc(sizeof(boundedQueue));
    queue->id = id;
    queue->capacity = size;
    queue->numOfProducts = numOfProducts;
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
    queue->finished = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    sem_init(&queue->full, 0, 0);
    sem_init(&queue->empty, 0, size);
    queue->buffer = malloc(sizeof(newsItem) * (size + 1));
    queue->types[0] = 0;
    queue->types[1] = 0;
    queue->types[2] = 0;
    return queue;
}

void insertBounded(boundedQueue *queue, newsItem item) {
    sem_wait(&queue->empty);
    pthread_mutex_lock(&queue->mutex);
    queue->buffer[queue->rear] = item;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->count++;
    pthread_mutex_unlock(&queue->mutex);
    sem_post(&queue->full);
}

newsItem popBounded(boundedQueue *queue) {
    sem_wait(&queue->full);
    pthread_mutex_lock(&queue->mutex);
    newsItem item = queue->buffer[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->count--;
    pthread_mutex_unlock(&queue->mutex);
    sem_post(&queue->empty);
    return item;
}

