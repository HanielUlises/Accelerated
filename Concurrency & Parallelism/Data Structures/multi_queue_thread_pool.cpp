#include "multi_queue_thread_pool.h"
#include <iostream>
#include <random>

static thread_local std::mt19937 rng{std::random_device{}()};

ThreadPool::ThreadPool() {
    thread_count = std::thread::hardware_concurrency();
    if (thread_count == 0) thread_count = 1;

    std::cout << "Creating multi-queue thread pool with " << thread_count << " worker threads\n";

    thread_data.resize(thread_count);
    for (unsigned int i = 0; i < thread_count; ++i) {
        thread_data[i] = std::make_unique<ThreadData>();
    }

    for (unsigned int i = 0; i < thread_count; ++i) {
        threads.emplace_back(&ThreadPool::worker, this, i);
    }
}

ThreadPool::~ThreadPool() {
    stopping = true;

    // Wake up all workers by pushing a dummy task to each local queue
    Func dummy = []{};
    for (auto& data : thread_data) {
        data->local_queue.push(dummy);
    }

    for (std::thread& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    std::cout << "Thread pool shut down cleanly\n";
}

void ThreadPool::worker(unsigned int my_index) {
    ThreadData& my_queue = *thread_data[my_index];
    std::uniform_int_distribution<unsigned int> dist(0, thread_count - 1);

    while (!stopping) {
        Func task;

        // 1. Try own queue (non-blocking)
        if (my_queue.local_queue.try_pop(task)) {
            task();
            continue;
        }

        // 2. Try to steal from other queues
        bool stole = false;
        unsigned int start = dist(rng);
        for (unsigned int i = 0; i < thread_count; ++i) {
            unsigned int victim = (start + i) % thread_count;
            if (victim == my_index) continue;

            if (thread_data[victim]->local_queue.try_pop(task)) {
                task();
                stole = true;
                break;
            }
        }

        if (stole) continue;

        // 3. Nothing found -> block on own queue
        my_queue.local_queue.pop(task);
        if (stopping) break;
        task();
    }
}

void ThreadPool::submit(Func func) {
    if (stopping) return;

    static std::atomic<unsigned int> next{0};
    unsigned int idx = next.fetch_add(1, std::memory_order_relaxed) % thread_count;

    thread_data[idx]->local_queue.push(std::move(func));
}