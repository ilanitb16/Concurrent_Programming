#include "Bounded_Buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "Dispatcher.h"
#include "Unbounded_Buffer.h"
using namespace std;

Dispatcher::Dispatcher(int producers, vector<Bounded_Buffer *> bufferIn, vector<Unbounded_Buffer *> bufferOut)
{
    this->producers = producers;
    this->bufferIn = bufferIn;
    this->bufferOut = bufferOut;
}
void Dispatcher::dispatch()
{
    int currentBufferIndex = 0; // Initialize current buffer index to 0
    int totalBuffers = bufferIn.size(); // Get the total number of input buffers
    int completedProducers = 0; // Initialize count of completed producers

    // Continue dispatching until all producers are done
    while (completedProducers < producers)
    {
        // Check if the current buffer is not empty
        if (!bufferIn[currentBufferIndex]->isEmpty())
        {
            char *message = bufferIn[currentBufferIndex]->remove(); // Remove an item from the current buffer
            int producerID, additionalValue; // Variables to store producer ID and additional integer value
            char messageType[100]; // Variable to store the type of message
            int itemsParsed = sscanf(message, "producer %d %99s %d", &producerID, messageType, &additionalValue); // Parse the input string

            // If the string was successfully parsed
            if (itemsParsed == 3)
            {
                // Check the type of the message and insert it into the appropriate output buffer
                if (strcmp(messageType, "SPORTS") == 0)
                {
                    bufferOut[0]->insert(message); // Insert into SPORTS buffer
                }
                else if (strcmp(messageType, "NEWS") == 0)
                {
                    bufferOut[1]->insert(message); // Insert into NEWS buffer
                }
                else if (strcmp(messageType, "WEATHER") == 0)
                {
                    bufferOut[2]->insert(message); // Insert into WEATHER buffer
                }
            }
            // If the message is "DONE", increment the count of completed producers
            if (strcmp(message, "DONE") == 0)
                completedProducers++;
        }
        // Move to the next buffer index, wrapping around if necessary
        currentBufferIndex = (currentBufferIndex + 1) % totalBuffers;
    }

    // Insert "DONE" messages into all output buffers to indicate completion
    char doneMessage[]= "DONE";
    bufferOut[0]->insert(doneMessage);
    bufferOut[1]->insert(doneMessage);
    bufferOut[2]->insert(doneMessage);
    return;
}
