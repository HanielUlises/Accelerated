#include <random>
#include <thread>
#include <iostream>

thread_local std::mt19937 mt;

void func() {
    std::uniform_real_distribution<double> dist(0,1);
    for(int i = 0; i < 10; i++)
        std::cout << dist(mt) << ", ";
}

int main() {
    std::cout << "Thread 1 random values: " << std::endl;
    std::thread thread_1 (func);
    thread_1.join();

    std::cout << "Thread 2 random values: " << std::endl;
    std::thread thread_2 (func);
    thread_2.join();
}
