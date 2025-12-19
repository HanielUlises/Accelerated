#include <iostream>
#include <thread>
#include <vector>
#include <numeric>

const int iterations = 100000;
int counter = 0;

void increment_counter(){
    for(int i = 0; i < iterations; i++){
        counter++;
    }
}

int main() {
    long long expected_value = 0;
    const int num_threads = 10;

    expected_value = static_cast<int> (num_threads * iterations);

    std::vector<std::thread> threads;

    for(int i = 0; i < num_threads; i++){
        threads.emplace_back(increment_counter);
    }

    for(auto &thread: threads){
        thread.join();
    }
    
    std::cout << "Expected final counter value: " << expected_value << std::endl;
    std::cout << "Actual final counter value:   " << counter << std::endl;

}