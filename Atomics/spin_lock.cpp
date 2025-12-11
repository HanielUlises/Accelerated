#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

std::atomic_flag lock_cout = ATOMIC_FLAG_INIT;

void task(int x) {
    while(lock_cout.test_and_set()) {

    }

    // Critical section
    using namespace std::literals;
    std::this_thread::sleep_for(50ms);

    std::cout << "I'm a task with argument " << x << '\n';

    // Clearing the flag allows other thread to set values;
    lock_cout.clear();
}

auto main () -> int {
    std::vector<std::thread> threads;

    for(size_t i = 1; i <= 10; i++) 
        threads.emplace_back(task, i);

    for(auto &thread: threads)
        thread.join();

    return 0;
}