#include <iostream>
#include <future>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>

#include "concurrent_queue.h"

#include "single_queue_thread_pool.h"
const char* pool_type = "Single-Queue Thread Pool";

#include "multi_queue_thread_pool.h"
const char* pool_type = "Multi-Queue (Work-Stealing) Thread Pool";

using namespace std::chrono_literals;

// Simple task for thread pool testing
void task(int id) {
    std::cout << "Task " << id 
              << " running on thread " << std::this_thread::get_id() << "\n";
    std::this_thread::sleep_for(100ms);
    std::cout << "Task " << id << " finished\n";
}

#ifdef TEST_QUEUE
concurrent_queue<std::string> conc_queue(10);
void writer() {
    using namespace std::literals;
    std::this_thread::sleep_for(1s); 

    std::cout << "\n[Queue Test] Writer starting to push 15 items...\n";
    for (int i = 0; i < 15; ++i) {
        std::string item = "Data-" + std::to_string(i);
        conc_queue.push(item);
        std::cout << "[Writer] Pushed: " << item << "\n";
        std::this_thread::sleep_for(50ms);
    }
    std::cout << "[Writer] Finished pushing\n";
}

void reader() {
    std::cout << "\n[Queue Test] Reader waiting for items...\n";
    for (int i = 0; i < 15; ++i) {
        std::string item;
        conc_queue.pop(item);
        std::cout << "[Reader] Popped: " << item << "\n";
        std::this_thread::sleep_for(80ms);
    }
    std::cout << "[Reader] Finished reading\n";
}
#endif

int main() {
    std::cout << "=== Starting " << pool_type << " ===\n\n";

    ThreadPool pool;

    const int num_tasks = 20;
    std::atomic<int> completed_tasks{0};

    // Submit tasks
    for (int i = 0; i < num_tasks; ++i) {
        pool.submit([i, &completed_tasks] {
            task(i);
            ++completed_tasks;
        });
    }

    // Submit a final task that runs when (most) others are likely done
    pool.submit([&pool, &completed_tasks, num_tasks] {
        while (completed_tasks.load() < num_tasks) {
            std::this_thread::sleep_for(50ms);
        }
        std::cout << "\nAll " << num_tasks << " tasks have completed!\n";
        std::cout << "Thread pool still alive on thread " << std::this_thread::get_id() << "\n";
    });

#ifdef TEST_QUEUE
    std::cout << "\n--- Running concurrent_queue test ---\n";
    auto fut_writer = std::async(std::launch::async, writer);
    auto fut_reader = std::async(std::launch::async, reader);

    fut_writer.get();
    fut_reader.get();
    std::cout << "--- Queue test completed ---\n\n";
#endif

    std::this_thread::sleep_for(3s);

    std::cout << "\nmain() exiting - thread pool will shut down\n";
    return 0;
}