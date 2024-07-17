#include "CoEditor.h"
#include <iostream>
#include <string>
#include <vector>
#include "Unbounded_Buffer.h"
#include <thread>
#include <unistd.h>
using namespace std;

// Constructor for CoEditor
CoEditor::CoEditor(Unbounded_Buffer *bufferIn, Unbounded_Buffer *bufferOut)
{
    this->bufferIn = bufferIn;
    this->bufferOut = bufferOut;
}

// Function that simulates the editing process
void CoEditor::edit()
{
    while (true)
    {
        // Check if the input buffer is not empty
        if (!bufferIn->isEmpty())
        {
            char *s = bufferIn->remove(); // Remove a message from the input buffer

            // If the message is "DONE", insert it into the output buffer and exit the function
            if (strcmp(s, "DONE") == 0)
            {
                bufferOut->insert(s);
                return;
            }
            else
            {
                usleep(100000); // Simulate editing delay
                bufferOut->insert(s); // Insert the edited message into the output buffer
            }
        }
    }
    return;
}