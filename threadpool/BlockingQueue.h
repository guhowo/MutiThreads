#ifndef THREADPOOL_BLOCKINGQUEUE_H
#define THREADPOOL_BLOCKINGQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class BlockingQueue {
public:
    BlockingQueue(){}

    inline void post(const T t)
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    inline T get()
    {
        std::unique_lock<std::mutex> lock(m);
        while (q.empty()) {
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

private:
    std::mutex m;
    std::condition_variable c;
    std::queue<T> q;
};

#endif //THREADPOOL_BLOCKINGQUEUE_H
