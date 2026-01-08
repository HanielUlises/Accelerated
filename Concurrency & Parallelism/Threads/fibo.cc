#include <iostream>
#include <thread>
#include <vector>
#include <type_traits>


template <int N>
struct Fib : std::integral_constant<int, Fib<N - 1>::value + Fib<N - 2>::value> {};

template <>
struct Fib<0> : std::integral_constant<int, 0> {};

template <>
struct Fib<1> : std::integral_constant<int, 1> {};


template <int N>
struct ThreadCount {
    static constexpr int value = (N > 8 ? 8 : N);
};


void worker(int id) {
    std::cout << "Thread " << id
              << " running on thread id "
              << std::this_thread::get_id() << '\n';
}


int main() {
    constexpr int fib_value = Fib<7>::value;
    constexpr int num_threads = ThreadCount<fib_value>::value;

    std::cout << "Compile-time Fib(7): " << fib_value << '\n';
    std::cout << "Launching " << num_threads << " threads\n\n";

    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
