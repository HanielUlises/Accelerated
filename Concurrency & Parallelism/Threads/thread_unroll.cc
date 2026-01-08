#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <functional>

template <int Outer, int... Rest>
struct ThreadUnroll {
    template <typename F, typename... Args>
    static void apply(F&& func, Args&&... args) {
        constexpr int num_threads = Outer;
        std::vector<std::thread> threads;

        threads.reserve(num_threads);

        for (int i = 0; i < Outer; ++i) {
            threads.emplace_back([i, &func, &args...]() {
                ThreadUnroll<Rest...>::apply(std::forward<F>(func), args..., i);
            });
        }

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
};

// Base case: innermost dimension
template <>
struct ThreadUnroll<> {
    template <typename F, typename... Args>
    static void apply(F&& func, Args&&... args) {
        func(args...);
    }
};

void process(int x, int y, int z) {
    double sum = 0.0;
    for (int k = 0; k < 1000; ++k) {
        sum += std::sin(x + k) * std::cos(y + k) * std::tan(z + k * 0.1);
    }
    static thread_local double accumulator = 0.0;
    accumulator += sum;
}

int main() {
    constexpr int NX = 4;
    constexpr int NY = 100;
    constexpr int NZ = 50;

    std::cout << "Running threaded 3D grid computation: "
              << NX << " x " << NY << " x " << NZ << " = "
              << (NX * NY * NZ) << " cells\n";
    std::cout << "Using " << NX << " std::threads (one per outer slice)\n";

    auto start = std::chrono::high_resolution_clock::now();

    ThreadUnroll<NX, NY, NZ>::apply([](int x, int y, int z) {
        process(x, y, z);
    });

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Computation finished in " << duration.count() << " ms\n";

    return 0;
}