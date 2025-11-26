#include <iostream>
#include <thread>
#include <mutex>
#include <string> 
#include <chrono>

using namespace std::literals;

std::mutex m1;
std::mutex m2;

void func_A() {
    std::cout << "Thread A trying to lock mutexes 1 and 2... \n";
    std::lock(m1, m2);
    std::cout << "Thread A has locked mutexes 1 and 2.\n";

    // Adopt the locks
    std::unique_lock<std::mutex> uniq_lck_1 (m1, std::adopt_lock);
    std::unique_lock<std::mutex> uniq_lck_2 (m2, std::adopt_lock);
    std::cout << "Thread A has adopted the locks.\n";

    std::this_thread::sleep_for(50ms);
    std::cout << "Thread A releasing mutexes.\n";
}

void func_B() {
    std::cout << "Thread B trying to lock mutexes 1 and 2... \n";
    std::lock(m1, m2);
    std::cout << "Thread B has locked mutexes 1 and 2.\n";

    // Adopt the locks
    std::unique_lock<std::mutex> uniq_lck_1 (m1, std::adopt_lock);
    std::unique_lock<std::mutex> uniq_lck_2 (m2, std::adopt_lock);
    std::cout << "Thread B has adopted the locks.\n";

    std::this_thread::sleep_for(50ms);
    std::cout << "Thread B releasing mutexes.\n";
}