#include <iostream>
#include <vector>
#include <chrono>
#include "prime_sieve.h"

int main() {
    const long long N = 500'000'000;           // Primes up to 500 million
    const int threads = std::thread::hardware_concurrency();

    std::cout << "Finding all primes up to " << N << " using " << threads << " threads...\n";

    auto start = std::chrono::high_resolution_clock::now();
    auto primes = parallel_sieve(N, threads);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Found " << primes.size() << " prime numbers.\n";
    std::cout << "First 10 primes: ";
    for (size_t i = 0; i < std::min<size_t>(10, primes.size()); ++i)
        std::cout << primes[i] << " ";
    std::cout << "\n";

    std::cout << "Last 10 primes:  ";
    for (size_t i = primes.size() > 10 ? primes.size() - 10 : 0; i < primes.size(); ++i)
        std::cout << primes[i] << " ";
    std::cout << "\n";

    std::cout << "Time taken: " << duration.count() << " ms\n";

    return 0;
}