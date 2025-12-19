#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <mutex>
#include <iostream>
#include <queue>
#include <thread>
#include <condition_variable>

struct concurrent_queue_empty : public std::runtime_error {
    public:
        concurrent_queue_empty() : std::runtime_error("Queue is empty") {}
        concurrent_queue_empty(const char *s) : std::runtime_error(s) {}
};

struct concurrent_queue_full : public std::runtime_error {
    public:
        concurrent_queue_full() : std::runtime_error("Queue is full") {}
        concurrent_queue_full(const char *s) : std::runtime_error(s) {}
};

template <typename T>
class concurrent_queue{
        std::mutex mut;
        std::queue<T> q;

        std::condition_variable cond_var;

        int max{50};
    
    public:
        concurrent_queue() = default;
        concurrent_queue(int _max) : max(_max) {}
        concurrent_queue& operator=(const concurrent_queue&) = delete;
        concurrent_queue(const concurrent_queue&) = delete;
        concurrent_queue& operator=(const concurrent_queue&&) = delete;

        // Push element
        void push(T value) {
            std::unique_lock<std::mutex> uniq_lck(mut);
            while(q.size() > max) {
                uniq_lck.unlock();
                std::this_thread::sleep_for(50ms);
                uniq_lck.lock();
            }

            q.push(value);
            cond_var.notify_one();
        }

        // Pop element
        void pop(T &value) {
            std::unique_lock<std::mutex> lck_grd(mut);

            cond_var.wait(lck_grd, [this] {return !q.empty()});

            value = q.front();
            q.pop();
        }
};

#endif