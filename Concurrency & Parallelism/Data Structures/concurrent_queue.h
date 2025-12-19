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
class concurrent_queue {
private:
    mutable std::mutex mut;
    std::queue<T> q;
    std::condition_variable cond_not_empty;
    std::size_t capacity;
    bool closed{false};

public:
    explicit concurrent_queue(std::size_t cap = 0) : capacity(cap) {}

    concurrent_queue(const concurrent_queue&) = delete;
    concurrent_queue& operator=(const concurrent_queue&) = delete;
    concurrent_queue(concurrent_queue&&) = delete;
    concurrent_queue& operator=(concurrent_queue&&) = delete;

    void push(T value) {
        std::unique_lock<std::mutex> lock(mut);
        if (closed) {
            throw std::runtime_error("push to closed queue");
        }
        if (capacity > 0) {
            cond_not_empty.wait(lock, [this] { return q.size() < capacity || closed; });
            if (closed) return;
        }
        q.push(std::move(value));
        lock.unlock();
        cond_not_empty.notify_one();
    }

    void pop(T& value) {
        std::unique_lock<std::mutex> lock(mut);
        cond_not_empty.wait(lock, [this] { return !q.empty() || closed; });
        if (q.empty()) {
            if (closed) {
                throw std::runtime_error("pop from closed queue");
            }
            return;
        }
        value = std::move(q.front());
        q.pop();
        lock.unlock();
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mut);
        if (q.empty()) {
            return false;
        }
        value = std::move(q.front());
        q.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mut);
        return q.empty();
    }

    void close() {
        std::lock_guard<std::mutex> lock(mut);
        closed = true;
        cond_not_empty.notify_all();
    }
};

#endif