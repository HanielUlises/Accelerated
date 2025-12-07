#include <atomic>
#include <thread>
#include <iostream>

int main() {
    std::atomic<int> counter{0};       // This is already lock-free on all normal CPUs

    auto worker = [&] {
        for (int i = 0; i < 10'000'000; ++i)
            counter.fetch_add(1, std::memory_order_relaxed);
    };

    std::thread t1(worker);
    std::thread t2(worker);

    t1.join();
    t2.join();

    std::cout << "Final value = " << counter << " (should be 20,000,000)\n";
}