#include <iostream>
#include <thread>
#include <mutex>
#include <string> 
#include <chrono>

using namespace std::literals;

std::mutex m1;
std::mutex m2;

void func_A() {
    std::unique_lock<std::mutex> uniq_lock_1 (m1, std::defer_lock);
    std::unique_lock<std::mutex> uniq_lock_2 (m2, std::defer_lock);

    std::cout << "Thread A trying to lock mutexes 1 and 2... \n";

    auto idx = std::try_lock(uniq_lock_1, uniq_lock_2);
    if(idx != - 1) {
        std::cout << "try_locl failed to lock mutex on index " << idx << '.\n'; 
    } else {
        std::cout << "Thread A has locked mutexes 1 and 2.\n";
        std::this_thread::sleep_for(500ms);
        std::cout << "Thread A releasing mutexes 1 and 2.\n";
    }
}