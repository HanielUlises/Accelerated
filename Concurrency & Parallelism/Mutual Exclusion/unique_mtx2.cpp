#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std::literals;

std::timed_mutex mtx;

void task1() {
    std::cout << "Task 1 trying to lock the mutex" << std::endl;
    std::lock_guard<std::timed_mutex> lck_guard (mtx);
    std::cout << "Task 1 has locked the mutex" << std::endl;
    std::this_thread::sleep_for(5s);
    std::cout << "Task 1 unlocking the mutex" << std::endl;
}

void task2() {
    std::this_thread::sleep_for(500ms);
    std::cout << "Thread 2 trying to lock the mutex" << std::endl;

    std::unique_lock<std::timed_mutex> uniq_lck (mtx, std::defer_lock);
    
    // Try for 1 second to lock the mutex
    while(!uniq_lck.try_lock_for(1s)){
        std::cout << "Thread 2 failed to lock the mutex" << std::endl;
    }

    // Locked
    std::cout << "Task2 has locked the mutex" << std::endl;
}

int main() {
    std::cout << "Starting threads..." << std::endl;

    std::thread t1(task1);
    std::thread t2(task2);

    t1.join();
    t2.join();

    std::cout << "Both threads have finished." << std::endl;

    return 0;
}

