#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <vector>

std::shared_mutex shrd_mut;

unsigned int x = 0;

void write(){
    std::lock_guard<std::shared_mutex> lck_guard(shrd_mut);
    x++;
}

void read() {
    std::shared_lock<std::shared_mutex> lck_guard(shrd_mut);
    using namespace std::literals;
    std::this_thread::sleep_for(100ms);
}

int main() {
    std::vector<std::thread> threads;

    for(int i = 0; i < 20; i++){
        threads.emplace_back([](){read();});
    }

    threads.emplace_back([](){write();});
    threads.emplace_back([](){write();});

    for(int i = 0; i < 20; i++){
        threads.emplace_back([](){read();});
    }

    for(auto &thr: threads){
        thr.join();
    }
}