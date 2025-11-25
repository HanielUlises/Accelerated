#include "parallel_sum.h"
#include <thread>
#include <vector>
#include <algorithm>
#include <iostream>

void partial_sum(const double* data, size_t start, size_t end, double& result) {
    double sum = 0.0;
    for (size_t i = start; i < end; ++i) {
        sum += data[i];
    }
    result = sum;
}

double parallel_sum(const double* data, size_t size, int num_threads) {
    if (num_threads <= 0) num_threads = 1;
    if (size < static_cast<size_t>(num_threads)) num_threads = 1;

    int hardware_threads = std::thread::hardware_concurrency();
    num_threads = std::min(num_threads, hardware_threads != 0 ? hardware_threads : 4);

    std::vector<std::thread> threads;
    std::vector<double> partial_sums(num_threads, 0.0);

    size_t chunk_size = size / num_threads;
    size_t remaining = size % num_threads;

    size_t start = 0;
    for (int i = 0; i < num_threads; ++i) {
        size_t extra = (i < remaining) ? 1 : 0;
        size_t end = start + chunk_size + extra;

        if (end > size) end = size;

        threads.emplace_back(partial_sum, data, start, end, std::ref(partial_sums[i]));

        start = end;
    }

    for (auto& t : threads) {
        t.join();
    }

    double total = 0.0;
    for (double ps : partial_sums) {
        total += ps;
    }

    return total;
}