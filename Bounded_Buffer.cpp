#include <queue>
#include <string>
#include <mutex>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <cstring>
#include <semaphore.h>
#include "Bounded_Buffer.h"
using namespace std;

Bounded_Buffer::Bounded_Buffer(int size) : size(size), buffer(queue<string>()) {}
void Bounded_Buffer::insert(char *s)
{
    std::unique_lock<std::mutex> lock(mtx);
    not_full.wait(lock, [this]() { return buffer.size() < size; });
    buffer.push(s);
    not_empty.notify_one();
}
char *Bounded_Buffer::remove()
{
    std::unique_lock<std::mutex> lock(mtx);
    not_empty.wait(lock, [this]() { return !buffer.empty(); });
    std::string s = buffer.front();
    buffer.pop();
    not_full.notify_one();
    char *result = new char[s.length() + 1];
    std::strcpy(result, s.c_str());
    return result;
}
bool Bounded_Buffer:: isEmpty(){
    unique_lock<std::mutex> lock(mtx);
    return buffer.empty();
}
