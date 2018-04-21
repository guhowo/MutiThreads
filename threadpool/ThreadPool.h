#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <vector>
#include "Thread.h"
#include "BlockingQueue.h"
#include <mutex> 

class ThreadPool {
public:
    typedef boost::function<void ()>  Task;

    ThreadPool();

    ~ThreadPool();

    void start(int numThreads);

    void run(const Task &);

    void runInThread();

private:
    BlockingQueue<Task> _qe;
    std::vector<Thread> _threads;
    bool _running;  //threads started
    std::mutex _m;
};

#endif //THREADPOOL_THREADPOOL_H
