#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H
#include "Unbounded_Buffer.h"
class ScreenManager
{
private:
    Unbounded_Buffer *bufferIn;
public:
    ScreenManager(Unbounded_Buffer *bufferIn);
    void display();
};
#endif