#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

int main() {
    std::future<void> task_1 = std::async(std::launch::async, [](){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Task 1 completed " << std::endl;
    }); 

    std::future<void> task_2 = std::async(std::launch::async, [](){
        std::this_thread::sleep_for(std::chrono::milliseconds(700));
        std::cout << "Task 2 completed " << std::endl;
    }); 

    std::future<void> task_3 = std::async(std::launch::async, [](){
        std::this_thread::sleep_for(std::chrono::milliseconds(900));
        std::cout << "Task 3 completed " << std::endl;
    }); 

    std::vector<std::future<void>> futures;
    futures.push_back(task_1);
    futures.push_back(task_2);
    futures.push_back(task_3);

    // Wait for tasks to complete
    for(auto &future: futures)
        future.get();

    std::cout << "All tasks completed" << std::endl;
    return 0;    
}