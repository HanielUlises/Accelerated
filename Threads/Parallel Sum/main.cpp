// main.cpp
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "parallel_sum.h"

int main() {
    const size_t array_size = 100'000'000;
    const int num_threads = std::thread::hardware_concurrency();
    
    std::vector<double> data(array_size);
    
    for (size_t i = 0; i < array_size; ++i) {
        data[i] = 1.0;
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    double result = parallel_sum(data.data(), array_size, num_threads);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Parallel Sum (using " << num_threads << " threads): " << result << std::endl;
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    double sequential_sum = 0.0;
    for (size_t i = 0; i < array_size; ++i) {
        sequential_sum += data[i];
    }
    end = std::chrono::high_resolution_clock::now();
    auto seq_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Sequential Sum: " << sequential_sum << std::endl;
    std::cout << "Sequential Time: " << seq_duration.count() << " ms" << std::endl;
    std::cout << "Speedup: " << (double)seq_duration.count() / duration.count() << "x" << std::endl;

    return 0;
}