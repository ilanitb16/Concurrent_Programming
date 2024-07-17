#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <cstring>
using namespace std;
#include "Unbounded_Buffer.h"

Unbounded_Buffer::Unbounded_Buffer(string name)
{
    this->name = name;
}
void Unbounded_Buffer::insert(char *s)
{
    unique_lock<mutex> lck(mtx);
    buffer.push(s);
    not_empty.notify_one();
}
char *Unbounded_Buffer::remove()
{
    unique_lock<mutex> lck(mtx);
    not_empty.wait(lck, [this]()
                   { return buffer.size() > 0; });
    string s = buffer.front();
    buffer.pop();
    char *result = new char[s.length() + 1];
    strcpy(result, s.c_str());
    return result;
}
bool Unbounded_Buffer::isEmpty()
{
    unique_lock<mutex> lck(mtx);
    return buffer.size() == 0;
}
