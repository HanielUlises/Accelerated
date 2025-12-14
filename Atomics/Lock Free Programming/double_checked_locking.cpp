#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>

class Test{
    public:
        void func() {
            std::cout << "Function calling..." << std::endl;
        }
};

std::atomic<Test*> p_test = nullptr;
std::mutex mut;

void process() {
    if(!p_test){
        std::lock_guard<std::mutex> lock_gr (mut);

        if(!p_test) {
            p_test = new Test;
        }
    }
    
    Test *ptr_test = p_test;
    ptr_test -> func();
}

int main() {
    std::vector<std::thread> threads;

    for(int i = 0; i < 10; i++) {
        threads.emplace_back(process);
    }

    for(auto &thr: threads) 
        thr.join();

    return;
}