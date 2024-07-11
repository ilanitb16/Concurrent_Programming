#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#include "unbounded_queue.h"
#include "bounded_queue.h"

#define NUM_OF_CONSUMERS 3
#define SCREEN_MANAGER 1000
#define SPORT_QUEUE 0
#define WEATHER_QUEUE 1
#define POLITICS_QUEUE 2

// Create a global array of bounded queues for producers
boundedQueue **producers;
unboundedQueue *consumers[NUM_OF_CONSUMERS];
boundedQueue *screenMonitorQueue;

// Print the type of the news item
void printType(newsItem item) {
    switch (item.type) {
        case sports:
            printf("SPORTS");
            break;
        case weather:
            printf("WEATHER");
            break;
        case news:
            printf("NEWS");
            break;
        default:
            printf("error\n");
            break;
    }
}

// Generate a random newsType
int generateRandomNewsType() {
    // Generate a random number between 0 and 2
    return rand() % 3;
}

// Create a news item
newsItem createNewsItem(int i) {
    newsItem item;
    item.id = i;
    item.type = generateRandomNewsType();
    if (i != -1) {
        item.numOfType = producers[i]->types[item.type];
        producers[i]->types[item.type]++;
    } else {
        item.numOfType = 0;
    }
    return item;
}

// Function declarations
void sortAndAdd(newsItem item);
void printNewsItem(newsItem item, int i);
void freeAll(int numOfProducers);

// Add a bounded queue to the global array of bounded queues
void addBoundedQueue(boundedQueue *queue, int numOfProducers) {
    // Reallocate the global array of bounded queues
    producers = realloc(producers, sizeof(boundedQueue) * (numOfProducers + 1));
    // Add the queue to the global array of bounded queues
    producers[numOfProducers - 1] = queue;
}

// Insert a news item into a bounded buffer
void insertBounded(boundedQueue *queue, newsItem item) {
    sem_wait(&queue->empty); // Wait until there is space in the queue
    pthread_mutex_lock(&queue->mutex); // Lock the queue

    // Insert the news item into the queue
    queue->buffer[queue->rear] = item;
    queue->rear = (queue->rear + 1) % queue->capacity; // Update the rear index
    queue->count++; // Increment the count of items in the queue

    pthread_mutex_unlock(&queue->mutex); // Unlock the queue
    sem_post(&queue->full); // Signal that the queue has new items
}

// Remove a news item from a bounded buffer
newsItem popBounded(boundedQueue *queue) {
    sem_wait(&queue->full); // Wait until there are items in the queue
    pthread_mutex_lock(&queue->mutex); // Lock the queue

    // Remove the news item from the queue
    newsItem item = queue->buffer[queue->front];
    queue->front = (queue->front + 1) % queue->capacity; // Update the front index
    queue->count--; // Decrement the count of items in the queue

    pthread_mutex_unlock(&queue->mutex); // Unlock the queue
    sem_post(&queue->empty); // Signal that there is space in the queue

    return item; // Return the news item
}

// Insert a news item into an unbounded queue
void insertUnbounded(unboundedQueue *queue, newsItem item) {
    Node *newNode = (Node *) malloc(sizeof(Node)); // Create a new node
    newNode->item = item;
    newNode->next = NULL;

    pthread_mutex_lock(&queue->mutex); // Lock the queue

    // Insert the item at the tail of the queue
    if (queue->tail == NULL) {
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }

    queue->count++; // Increment the count of items in the queue

    pthread_mutex_unlock(&queue->mutex); // Unlock the queue
    sem_post(&queue->full); // Signal that the queue has new items
}

// Remove a news item from an unbounded queue
newsItem popUnbounded(unboundedQueue *queue) {
    sem_wait(&queue->full); // Wait until there are items in the queue
    pthread_mutex_lock(&queue->mutex); // Lock the queue

    if (queue->head == NULL) {
        pthread_mutex_unlock(&queue->mutex); // Unlock the queue
        return createNewsItem(-1); // Return a default news item if the queue is empty
    }

    // Remove the item from the head of the queue
    Node *nodeToRemove = queue->head;
    newsItem item = nodeToRemove->item;
    queue->head = queue->head->next;

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    queue->count--; // Decrement the count of items in the queue
    free(nodeToRemove); // Free the memory for the removed node

    pthread_mutex_unlock(&queue->mutex); // Unlock the queue

    return item; // Return the news item
}

// Producer function
void *producer(void *args) {
    int *ptr = ((int *) args);
    int indexOfQueue = *ptr;

    for (int i = 0; i < producers[indexOfQueue]->numOfProducts; i++) {
        newsItem item = createNewsItem(indexOfQueue);
        insertBounded(producers[indexOfQueue], item); // Insert the news item into the producer's queue
    }

    producers[indexOfQueue]->finished = -1; // Mark the producer as finished
    return NULL;
}

// Dispatcher function
void *dispatcher(void *args) {
    int numOfProducers = *(int *) args;
    int totalProducts = 0;

    // Calculate the total number of products
    for (int i = 0; i < numOfProducers; i++) {
        totalProducts += producers[i]->numOfProducts;
    }

    int counter = 0;
    int isAllFinished = 0;

    // Array of flags to check if the producers are finished
    int *finishedFlags = malloc(sizeof(int) * numOfProducers);
    for (int i = 0; i < numOfProducers; i++) {
        finishedFlags[i] = 0;
    }

    int currNumOfNewsItems = 0;
    while (1) {
        for (counter = 0; counter < numOfProducers; counter++) {
            if (producers[counter]->finished == -1) {
                finishedFlags[counter] = 1;
                if (producers[counter]->count == 0) {
                    continue;
                }
            }

            // Remove the last news item from the producer's queue
            newsItem item = popBounded(producers[counter]);
            sortAndAdd(item); // Sort and add the news item to the appropriate queue
            currNumOfNewsItems++;
        }

        // Check if all producers are finished
        for (int i = 0; i < numOfProducers; i++) {
            if (finishedFlags[i] == 0) {
                isAllFinished = 0;
                break;
            }
            isAllFinished = 1;
        }

        // If all producers are finished and all news items are processed, break the loop
        if (isAllFinished == 1 && currNumOfNewsItems == totalProducts) {
            for (int i = 0; i < 3; i++) {
                consumers[i]->finished = -1;
            }
            break;
        }
    }

    free(finishedFlags); // Free the memory for finished flags
    return NULL;
}

// Sort and add the news item to the appropriate consumer queue
void sortAndAdd(newsItem item) {
    switch (item.type) {
        case sports:
            insertUnbounded(consumers[SPORT_QUEUE], item);
            break;
        case weather:
            insertUnbounded(consumers[WEATHER_QUEUE], item);
            break;
        case news:
            insertUnbounded(consumers[POLITICS_QUEUE], item);
            break;
    }
}

// Co-Editor function
void *coEditor(void *args) {
    int indexOfQueue = *(int *) args;

    while (1) {
        if (consumers[indexOfQueue]->count == 0) {
            if (consumers[indexOfQueue]->finished == -1) {
                break;
            }
            continue;
        }

        // Remove the last news item from the consumer's queue
        newsItem item = popUnbounded(consumers[indexOfQueue]);
        usleep(100000); // Simulate editing time
        insertBounded(screenMonitorQueue, item); // Insert the edited news item into the screen monitor queue
    }

    pthread_mutex_lock(&screenMonitorQueue->mutex);
    screenMonitorQueue->finished += 1; // Increment the finished counter for the screen monitor queue
    pthread_mutex_unlock(&screenMonitorQueue->mutex);

    return NULL;
}

// Screen monitor function
void *screenMonitor(void *args) {
    int numOfItems = 0;

    while (1) {
        if (screenMonitorQueue->count == 0) {
            if (screenMonitorQueue->finished == 3) {
                printf("DONE");
                break;
            }
            continue;
        }

        // Remove the last news item from the screen monitor queue
        newsItem item = popBounded(screenMonitorQueue);
        numOfItems++;
        fflush(stdout);
        printNewsItem(item, numOfItems); // Print the news item
    }

    return NULL;
}

// Print the news item
void printNewsItem(newsItem item, int i) {
    printf("PRODUCER %d ", item.id + 1);
    printType(item);
    printf(" %d\n", item.numOfType + 1);
}

// Create a bounded queue
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

// Initialize the unbounded queue
void initUnbounded(unboundedQueue *queue) {
    queue->id = 0;
    queue->tail = NULL;
    queue->head = NULL;
    queue->count = 0;
    queue->finished = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    sem_init(&queue->full, 0, 0);
}

// Create an unbounded queue
unboundedQueue *createUnboundedQueue(int id) {
    unboundedQueue *queue = (unboundedQueue *) malloc(sizeof(unboundedQueue));
    queue->id = id;
    initUnbounded(queue);
    return queue;
}

// Create the consumer queues
void createConsumersQueues() {
    for (int i = 0; i < NUM_OF_CONSUMERS; i++) {
        consumers[i] = createUnboundedQueue(i);
    }
}

int main(int argc, char *argv[]) {
    int numOfProducers = 0;
    int coEditorQueueSize = 0;

    // Open the configuration file
    FILE *configFile = fopen(argv[1], "r");
    if (configFile == NULL) {
        printf("Failed to open the config file.\n");
        return 1;
    }

    int producerId, numProducts, boundedQueueSize;
    while (fscanf(configFile, "%d%d%d", &producerId, &numProducts, &boundedQueueSize) == 3) {
        numOfProducers++;
        addBoundedQueue(createBoundedQueue(producerId, boundedQueueSize, numProducts), numOfProducers);
    }

    // Close the configuration file
    fclose(configFile);

    // Last line of the config file is the size of the co-editors queue
    coEditorQueueSize = producerId;
    screenMonitorQueue = createBoundedQueue(SCREEN_MANAGER, coEditorQueueSize, 0);
    createConsumersQueues();

    // Create the producer threads
    pthread_t *producerTid = malloc(sizeof(pthread_t) * numOfProducers);
    int queueIndices[numOfProducers];
    for (int j = 0; j < numOfProducers; j++) {
        queueIndices[j] = j;
        if (pthread_create(&producerTid[j], NULL, producer, (void *)&queueIndices[j]) != 0) {
            perror("failed to create producer thread");
        }
    }

    // Create the dispatcher thread
    pthread_t dispatcherTid;
    if (pthread_create(&dispatcherTid, NULL, dispatcher, (void *) &numOfProducers) != 0) {
        perror("failed to create dispatcher thread");
    }

    // Create the consumer (co-editor) threads
    pthread_t newsUnboundedTid[NUM_OF_CONSUMERS];
    int consumerIds[NUM_OF_CONSUMERS] = {0, 1, 2};
    for (int i = 0; i < NUM_OF_CONSUMERS; i++) {
        if (pthread_create(&newsUnboundedTid[i], NULL, coEditor, (void *) &consumerIds[i]) != 0) {
            perror("failed to create consumer thread");
        }
    }

    // Create the screen monitor thread
    pthread_t monitorTid;
    if (pthread_create(&monitorTid, NULL, screenMonitor, NULL) != 0) {
        perror("failed to create monitor thread");
    }

    // Join the producer threads
    for (int i = 0; i < numOfProducers; i++) {
        if (pthread_join(producerTid[i], NULL) != 0) {
            perror("failed to join producer thread");
        }
    }

    // Join the dispatcher thread
    if (pthread_join(dispatcherTid, NULL) != 0) {
        perror("failed to join dispatcher thread");
    }

    // Join the consumer (co-editor) threads
    for (int i = 0; i < NUM_OF_CONSUMERS; i++) {
        if (pthread_join(newsUnboundedTid[i], NULL) != 0) {
            perror("failed to join consumer thread");
        }
    }

    // Join the screen monitor thread
    if (pthread_join(monitorTid, NULL) != 0) {
        perror("failed to join monitor thread");
    }

    freeAll(numOfProducers);
    free(producerTid);
    return 0;
}

// Free the memory allocated for the bounded queue
void freeBoundedQueue(boundedQueue *queue) {
    free(queue->buffer);
    pthread_mutex_destroy(&queue->mutex);
    sem_destroy(&queue->full);
    sem_destroy(&queue->empty);
    free(queue);
}

// Free the memory allocated for the unbounded queue
void freeUnboundedQueue(unboundedQueue *queue) {
    Node *current = queue->head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    pthread_mutex_destroy(&queue->mutex);
    sem_destroy(&queue->full);
    free(queue);
}

// Free all allocated resources
void freeAll(int numOfProducers) {
    for (int i = 0; i < numOfProducers; i++) {
        freeBoundedQueue(producers[i]);
    }
    free(producers);
    for (int i = 0; i < NUM_OF_CONSUMERS; i++) {
        freeUnboundedQueue(consumers[i]);
    }
    free(screenMonitorQueue);
}
