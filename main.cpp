#include <stdio.h>
#include "Bounded_Buffer.h"
#include "Dispatcher.h"
#include "Producer.h"
#include "ScreenManager.h"
#include "CoEditor.h"
#include "Unbounded_Buffer.h"
#include <vector>
#include <thread>
#include <iostream>
#include <string>

using namespace std;
int main(int argc, char *argv[])
{
    vector <Bounded_Buffer*> bufferIn;
    vector <Producer*> producers;
    vector <Unbounded_Buffer*> bufferOut;
    vector <CoEditor*> coeditors;
    vector <thread> threads;
    int numProducers = 0;
    char* file1 = argv[1];
    FILE *file = fopen(file1, "r");
    if(file == NULL)
    {
        perror("error opening file");
        return 1;
    }
    char line[256];
    int producerNumber, numberOfProducts, queueSize;
    while(fgets(line, sizeof(line), file)!=NULL)
    {
        // Check for "PRODUCER" line and parse it
        if (sscanf(line, "PRODUCER %d", &producerNumber) == 1) {
            // Successfully parsed a producer line
            // Assuming the next line is the number of products
            if (fgets(line, sizeof(line), file) != NULL && sscanf(line, "%d", &numberOfProducts) == 1) {
            }
            // Assuming the next line is the queue size
            if (fgets(line, sizeof(line), file) != NULL && sscanf(line, "queue size = %d", &queueSize) == 1) {
                Bounded_Buffer *b = new Bounded_Buffer(queueSize);
                bufferIn.push_back(b);
                Producer *p = new Producer(producerNumber, numberOfProducts, b);
                producers.push_back(p);
                numProducers++;
            }
        }
    }
    fclose(file);
    // SPORT NEWS WEATHER BUFFERS
    Unbounded_Buffer *sports = new Unbounded_Buffer("sports");
    Unbounded_Buffer *news = new Unbounded_Buffer("news");
    Unbounded_Buffer *weather = new Unbounded_Buffer("weather");
    bufferOut.push_back(sports);
    bufferOut.push_back(news);
    bufferOut.push_back(weather);
    // MANAGER INPUT BUFFER 
    Unbounded_Buffer *manager = new Unbounded_Buffer("manager");
    // SCREEN MANAGER
    ScreenManager *sm = new ScreenManager(manager);
    // COEDITORS
    CoEditor *sports_editor = new CoEditor(sports,manager);
    CoEditor *news_editor = new CoEditor(news,manager);
    CoEditor *weather_editor = new CoEditor(weather,manager);
    coeditors.push_back(sports_editor);
    coeditors.push_back(news_editor);
    coeditors.push_back(weather_editor);
    // DISPATCHER
    Dispatcher *d = new Dispatcher(numProducers,bufferIn,bufferOut);

    // RUN ALL THREADS
    // producer run
    for(Producer *p : producers)
    {
        threads.push_back(thread(&Producer::produce, p));
    }
    // dispatcher run
    threads.push_back(thread(&Dispatcher::dispatch, d));
    // coeditor run
    for(CoEditor *c : coeditors)
    {
        threads.push_back(thread(&CoEditor::edit, c));
    }
    // screen manager run
    threads.push_back(thread(&ScreenManager::display, sm));

    // join all threads
    for(thread &t : threads)
    {
        t.join();
    }
    return 0;
}