#ifndef _ATOMIC_QUEUE
#define _ATOMIC_QUEUE

#include <queue>
#include <mutex>

using namespace std;

template<class T>
class AtomicQueue{
    public:
        void push(T&& element){
            lock_guard<mutex> lock(m);
            q.push(element);
        }

        bool pop(T& out){
            lock_guard<mutex> lock(m);
            if(q.empty())
                return false;
            out = q.front();
            q.pop();
            return true;
        }
    private:
        queue<T> q;
        mutex m;
};
#endif
