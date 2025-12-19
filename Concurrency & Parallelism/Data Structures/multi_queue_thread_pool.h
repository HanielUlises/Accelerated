#ifndef MULTI_QUEUE_THREAD_POOL_H
#define MULTI_QUEUE_THREAD_POOL_H

#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include <memory>
#include "concurrent_queue.h"

using Func = std::function<void()>;

/**
 * Multi-queue work-stealing thread pool.
 * Each worker has its own local queue.
 * Idle workers steal tasks from other workers' queues.
 */
class ThreadPool {
private:
    struct ThreadData {
        concurrent_queue<Func> local_queue;
    };

    std::vector<std::thread> threads;
    std::vector<std::unique_ptr<ThreadData>> thread_data;

    std::atomic<bool> stopping{false};
    unsigned int thread_count;

    void worker(unsigned int my_index);

public:
    ThreadPool();
    ~ThreadPool();

    void submit(Func func);
};

#endif // MULTI_QUEUE_THREAD_POOL_H