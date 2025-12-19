#ifndef SINGLE_QUEUE_THREAD_POOL_H
#define SINGLE_QUEUE_THREAD_POOL_H

#include <thread>
#include <functional>
#include <atomic>

#include "concurrent_queue.h"

using Func = std::function<void()>;

class ThreadPool{
        // Queue of tasks functions
        concurrent_queue<Func> work_queue;
        // Thread objects that will make the pool
        std::vector<std::thread> threads;
        // Entry point for threads
        void worker();

        int thread_count;
        std::atomic<bool> stopping{false};
    public:
        ThreadPool();
        ~ThreadPool();

        void submit(Func func);
};

#endif