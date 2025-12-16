#include <future>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>

using namespace std::literals;

int task() {
    std::cout << "Executing task() in thread with ID: " << std::this_thread::get_id() << '\n';
    std::this_thread::sleep_for(5s);
    std::cout << "Returning from task()\n";
    return 42;
}

void func(const std::string option = "default"s) {
    std::future<int> result_;

    if(option == "async"s) {
        result_ = std::async(std::launch::async, task);
    } else if(option == "deferred"s) {
        result_ = std::async(std::launch::deferred, task);
    } else {
        result_ = std::async(task);
    }

    std::cout << "Calling async with option \"" << option << "\"\n";
    std::this_thread::sleep_for(2s);
    std::cout << "Calling get() method\n";
    std::cout << "Task result: " << result_.get() << '\n';
}

int main() {
    std::cout <<  "From main thread with ID: " << std::this_thread::get_id() << '\n';

    func("async");
    func("deferred");
    func("default");
}