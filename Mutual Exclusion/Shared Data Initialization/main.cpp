#include <thread>
#include <vector>

#include "singleton.h"

void task() {
    Singleton& single = get_singleton();
    std::cout << &single << std::endl;
}

int main() {   
    std::vector<std::thread> threads;
    for(int i = 0; i < 10; i++)
        threads.emplace_back(task);
    
    for(auto &thr: threads)
        thr.join();
}