#include <AtomicQueue.hpp>

using namespace std;

template <class T>
void AtomicQueue<T>::push(T&& element){
    lock_guard<mutex> lock(m);
    q.push(element);
}

template <class T>
bool AtomicQueue<T>::pop(T& out){
    lock_guard<mutex> lock(m);
    if(q.empty())
        return false;
    out = q.front();
    q.pop();
    return true;
}

