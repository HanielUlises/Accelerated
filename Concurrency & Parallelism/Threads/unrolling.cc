#include <omp.h>
#include <iostream>
#include <chrono>
#include <vector>   
#include <cmath>

template <int Outer, int... Rest>
struct ParallelUnroll {
    template <typename F, typename... Args>
    static void apply(F&& func, Args&&... args) {
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < Outer; ++i) {
            ParallelUnroll<Rest...>::apply(std::forward<F>(func),
                                           std::forward<Args>(args)..., i);
        }
    }
};

template <>
struct ParallelUnroll<> {
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
    const int NX = 4;
    const int NY = 100;
    const int NZ = 50;

    std::cout << "Running parallel 3D grid computation: "
              << NX << " x " << NY << " x " << NZ << " = "
              << (NX * NY * NZ) << " cells\n";
    std::cout << "Using " << omp_get_max_threads() << " OpenMP threads\n";

    auto start = std::chrono::high_resolution_clock::now();

    ParallelUnroll<NX, NY, NZ>::apply([](int x, int y, int z) {
        process(x, y, z);
    });

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Computation finished in " << duration.count() << " ms\n";

    return 0;
}