#ifndef DISPATCHER_H
#define DISPATCHER_H
#include <string>
#include <vector>
#include "Bounded_Buffer.h"
#include "Unbounded_Buffer.h"
using namespace std;
class Dispatcher
{
private:
    int producers;
    vector<Bounded_Buffer *> bufferIn;
    vector<Unbounded_Buffer *> bufferOut;
public:
    Dispatcher(int producers, vector<Bounded_Buffer *> bufferIn, vector<Unbounded_Buffer *> bufferOut);
    void dispatch();
};
#endif // DISPATCHER_H