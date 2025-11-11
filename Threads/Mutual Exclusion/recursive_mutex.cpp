#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::recursive_mutex rm;  

unsigned long long factorial(int n) {
    std::lock_guard<std::recursive_mutex> lock(rm);  

    if (n <= 1)
        return 1;

    return n * factorial(n - 1);
}

int main() {

    std::cout << "Computing factorial threads\n";
    
    std::vector<std::thread> threads;

    for(int i = 0; i < 5; i++){
        int n = 3 + (i % 3);
        threads.emplace_back([n](){
            unsigned long long fact = factorial(n);
            {
                std::lock_guard<std::recursive_mutex> lock(rm);
                std::cout << "Factorial of " << n << " is " << fact << std::endl;
            }

        });
    }

    for(auto &thread: threads){
        thread.join();
    }

    std::cout << "\nAll done!\n";
    return 0;
}