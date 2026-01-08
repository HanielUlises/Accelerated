#include <iostream>
#include <thread>
#include <type_traits>

// TMP flag
template <bool EnableThreads>
struct Runner;

// Specialization: threading enabled
template <>
struct Runner<true> {
    static void run() {
        std::thread t([] {
            std::cout << "Running in a thread\n";
        });
        t.join();
    }
};

// Specialization: threading disabled
template <>
struct Runner<false> {
    static void run() {
        std::cout << "Running without threads\n";
    }
};

int main() {
    constexpr bool use_threads = true; // change to false

    Runner<use_threads>::run();
}
