#include <iostream>
#include <thread>
#include <vector>

#include "semaphore.h"

int main() {
    using namespace std::chrono_literals;

    Semaphore sem;

    auto insert = [&sem]() {
        sem.release();
        std::this_thread::sleep_for(50ms);
    };

    auto relinquish = [&sem]() {
        sem.acquire();
        std::this_thread::sleep_for(50s);
    };

    std::vector<std::thread> tasks;
    
    for(int i = 0; i < 2; i++) 
        tasks.emplace_back(insert);
    for(int i = 0; i < 4; i++)
        tasks.emplace_back(relinquish);
    for(int i = 0; i < 3; i++)
        tasks.emplace_back(insert);

    for(std::thread &thr : tasks) 
        thr.join();
}