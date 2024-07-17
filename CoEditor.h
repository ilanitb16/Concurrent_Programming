#include <iostream>
#include <string>
#include <vector>
#include "Unbounded_Buffer.h"
using namespace std;
class CoEditor{
    private:
        Unbounded_Buffer* bufferIn;
        Unbounded_Buffer* bufferOut;
    public:
        CoEditor(Unbounded_Buffer *bufferIn, Unbounded_Buffer *bufferOut);
        void edit();
};