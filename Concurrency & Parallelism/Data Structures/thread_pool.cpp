#include <iostream>
#include "single_queue_thread_pool.h"

ThreadPool::ThreadPool() {
    thread_count = std::thread::hardware_concurrency();
    if(thread_count > 1) thread_count -= 1;
    else thread_count = 1;

    std::cout << "Creating thread pool with " << thread_count << " threads\n";

    for(int i = 0; i < thread_count; i++)
        threads.emplace_back(&ThreadPool::worker, this);
}

ThreadPool::~ThreadPool() {
    stopping = true;

    for(int i = 0; i < thread_count; i++) {
        work_queue.push([]{}); 
    }

    for(std::thread &t : threads) {
        if(t.joinable())
            t.join();
    }
}

void ThreadPool::worker() {
    while(true) {
        Func task;
        work_queue.pop(task);  // blocks here until something pushed

        // After unblock, check if we should stop
        if(stopping) break;

        task();
    }
}

void ThreadPool::submit(Func func) {
    if(stopping) return;
    work_queue.push(std::move(func));
}