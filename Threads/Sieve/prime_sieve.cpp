#include "prime_sieve.h"

std::vector<long long> parallel_sieve(long long n, int num_threads) {
    if (n < 2) return {};

    if (num_threads <= 0)
        num_threads = std::thread::hardware_concurrency();
    num_threads = std::max(1, std::min(num_threads, (int)std::thread::hardware_concurrency()));

    long long sqrt_n = std::sqrt(n);
    std::vector<bool> is_prime(n + 1, true);
    is_prime[0] = is_prime[1] = false;

    // 1: Sequential sieving for small primes (up to sqrt(n))
    for (long long i = 2; i <= sqrt_n; ++i) {
        if (is_prime[i]) {
            for (long long j = i * i; j <= n; j += i) {
                is_prime[j] = false;
            }
        }
    }

    // 2: Parallel sieving for remaining multiples above sqrt(n)
    std::vector<std::thread> workers;

    auto mark_multiples = [&](long long start_prime) {
        for (long long p = start_prime; p <= sqrt_n; ++p) {
            if (is_prime[p]) {
                long long multiple = (n / p) * p;
                if (multiple < p * p) multiple = p * p;

                for (; multiple <= n; multiple += p) {
                    if (multiple != p) {
                        is_prime[multiple] = false;
                    }
                }
            }
        }
    };

    long long block_size = (sqrt_n - 2) / num_threads + 1;
    for (int i = 0; i < num_threads; ++i) {
        long long start = 2 + i * block_size;
        long long end   = (i + 1 == num_threads) ? sqrt_n + 1 : start + block_size;

        workers.emplace_back([&, start, end]() {
            for (long long p = start; p < end; ++p) {
                if (is_prime[p]) {
                    long long multiple = p * p;
                    while (multiple <= n) {
                        is_prime[multiple] = false;
                        multiple += p;
                        if ((multiple % 1024) == 0) std::this_thread::yield();
                    }
                }
            }
        });
    }

    for (auto& t : workers) t.join();

    std::vector<long long> primes;
    primes.reserve(n / 10); 
    for (long long i = 2; i <= n; ++i) {
        if (is_prime[i]) primes.push_back(i);
    }

    return primes;
}