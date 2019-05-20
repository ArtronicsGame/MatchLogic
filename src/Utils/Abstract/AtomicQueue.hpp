#ifndef _ATOMIC_QUEUE
#define _ATOMIC_QUEUE

#include <queue>
#include <mutex>

using namespace std;

template <class T>
class AtomicQueue{
    public:
        void push(T&& element);
        bool pop(T& out);
    private:
        queue<T> q;
        mutex m;
};

#include "AtomicQueue.cpp"

#endif
