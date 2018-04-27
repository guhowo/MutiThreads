#ifndef SEMPHORE_V3_BOUNDEDBLOCKINGQUEUE_H
#define SEMPHORE_V3_BOUNDEDBLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>

#include <boost/circular_buffer.hpp>

template <typename T>
class BoundedBlockingQueue {
public:
	explicit BoundedBlockingQueue(int size) : queue_(size) {}

    BoundedBlockingQueue(const BoundedBlockingQueue &) = delete;
    BoundedBlockingQueue &operator=(BoundedBlockingQueue &) = delete;

    void post(const T &a)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(queue_.full())
            notFull_.wait(lock);
        queue_.push_back(a);
        notEmpty_.notify_one();
    }

    T get()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(queue_.empty())
            notEmpty_.wait(lock);
        T front(queue_.front());
        queue_.pop_front();
        notFull_.notify_one();
        return front;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    bool full() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.full();
    }

    bool size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
    boost::circular_buffer<T> queue_;
};

#endif //SEMPHORE_V3_BOUNDEDBLOCKINGQUEUE_H
