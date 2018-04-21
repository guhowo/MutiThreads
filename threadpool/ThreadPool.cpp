#include <boost/bind.hpp>
#include "ThreadPool.h"

ThreadPool::ThreadPool() : _running(false)
{
}

ThreadPool::~ThreadPool()
{
    std::lock_guard<std::mutex> lock(_m);
    if (_running) {
        for (std::vector<Thread>::iterator iter = _threads.begin(); iter != _threads.end(); iter++)     //改成boost::bind
            iter->join();
    }
}

void ThreadPool::start(int numThreads)
{
    {
        std::lock_guard<std::mutex> lock(_m);
        if (!_running) {
            _threads.reserve(numThreads);
            for (int i = 0; i < numThreads; i++) {
                _threads.push_back(Thread(boost::bind(&ThreadPool::runInThread, this)));
                _threads[i].start((void *)0);      //static inline is right?
            }
            _running = true;
        }
    }
}

void ThreadPool::run(const Task &task)
{
    _qe.post(task);
}

void ThreadPool::runInThread()
{
    _qe.get();
}
