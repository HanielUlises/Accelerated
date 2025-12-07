// main.cpp - Tiny but useful lock-free coordination (C++20)

#include <atomic>
#include <thread>
#include <vector>
#include <iostream>

int main() {
    const int threads = std::thread::hardware_concurrency();
    const long long work_per_thread = 50'000'000LL;

    std::atomic<long long> shared_counter{0};   // the real data
    std::atomic<int>       workers_left{threads}; // countdown

    auto worker = [&] {
        // Phase 1: do a lot of work on the shared counter
        for (long long i = 0; i < work_per_thread; ++i) {
            shared_counter.fetch_add(1, std::memory_order_relaxed);
        }

        // Phase 2: announce "I'm done"
        if (workers_left.fetch_sub(1, std::memory_order_release) == 1) {
            // Last thread wakes the main thread
            std::cout << "All threads finished! Final counter = "
                      << shared_counter.load(std::memory_order_relaxed) << '\n';
        }
    };

    std::vector<std::thread> pool;
    for (int i = 0; i < threads; ++i)
        pool.emplace_back(worker);

    for (auto& t : pool) t.join();

    std::cout << "Main thread sees: " << shared_counter << '\n';
}