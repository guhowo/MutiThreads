#include <iostream>
#include <thread>
#include <mutex>
#include <functional>

#include "BoundedBlockingQueue.h"

int pindex = 0;

std::mutex mp;
std::mutex mc;

void *producer(BoundedBlockingQueue<int> &b)
{
    while(true) {
        {
            std::lock_guard<std::mutex> lock(mp);
            b.post(pindex++);
        }
    }
}

void *consumer(BoundedBlockingQueue<int> &b, int no)
{
    while(true) {
        {
            std::lock_guard<std::mutex> lock(mc);
            printf("consumer %d :%d", b.get(), no);
        }
    }
}

int main()
{
    BoundedBlockingQueue<int> b(10);
    auto pf1 = std::bind(&producer, b);
    auto pf2 = std::bind(&producer, b);
    auto cf1 = std::bind(&consumer, b, 1);
    auto cf2 = std::bind(&consumer, b, 2);

    std::thread producer1(pf1);
    std::thread producer2(pf2);
    std::thread consumer1(cf1);
    std::thread consumer2(cf2);

    producer1.join();
    producer2.join();
    consumer1.join();
    consumer2.join();

    return 0;
}