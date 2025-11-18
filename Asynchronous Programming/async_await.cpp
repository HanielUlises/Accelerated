#include <cppcoro/task.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/static_thread_pool.hpp>
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

// A coroutine that returns a task<T>
cppcoro::task<int> compute_async() {
    std::cout << "Starting long computation...\n";
    co_await cppcoro::schedule_on(cppcoro::static_thread_pool{}.schedule(), 2s);
    std::cout << "Computation done!\n";
    co_return 999;
}

cppcoro::task<std::string> fetch_data() {
    std::cout << "Fetching data from network...\n";
    co_await std::chrono::seconds(1);
    co_return "Data from server!";
}

cppcoro::task<void> main_async() {
    auto t1 = compute_async();
    auto t2 = fetch_data();

    std::cout << "Doing other work while waiting...\n";

    int result = co_await t1;
    std::string data = co_await t2;

    std::cout << "Result: " << result << "\n";
    std::cout << "Data: " << data << "\n";
}

int main() {
    cppcoro::sync_wait(main_async());
    return 0;
}