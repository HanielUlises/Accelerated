#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <utility>

// Pure function that performs a simulated computation
int compute_value(int input) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return input * 2;
}

// Pure function for further processing
int process_value(int input) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return input + 10;
}

// Launch a computation asynchronously
std::future<int> launch_async_task(std::function<int(int)> task_func, int input) {
    std::packaged_task<int(int)> task (task_func);
    std::future<int> future_result = task.get_future();

    std::thread t([task = std::move(task), input]() mutable { task (input);});
    t.detach();

    return future_result;
}

int main() {
    const int initial_value = 21;

    std::future<int> future_value = launch_async_task(compute_value, initial_value);

    std::cout << "Main function" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    int intermediate_val = future_value.get();
    std::cout << "Intermediate value: " << intermediate_val << std::endl;

    std::future<int> final_future = launch_async_task(process_value, intermediate_val);

    int final_result = final_future.get();

    std::cout << "Initial value: " << initial_value << std::endl;
    std::cout << "Final value: " << final_result << std::endl;
}