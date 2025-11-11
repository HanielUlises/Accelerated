#include <mutex>
#include <iostream>
#include <chrono>
#include <string>
#include <thread>

using namespace std::literals;

std::mutex print_mutex;

void task(std::string str){
    for(int i = 0; i < 5; i++){
        std::unique_lock<std::mutex> uniq_lck (print_mutex);
        std::cout << str[0] << str[1] << str[2] << std::endl;
        uniq_lck.unlock();
        std::this_thread::sleep_for(50ms);
    }
}

int main() {
    std::thread th1 (task, "abc");
    std::thread th2 (task, "def");
    std::thread th3 (task, "ghi");

    th1.join(); th2.join(); th3.join();
}