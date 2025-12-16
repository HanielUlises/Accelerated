#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <future>
#include <random>

static std::mt19937 mt;
std::uniform_real_distribution<double> dist(0, 100);

double sum_over(double *begin, double *end) {
    return std::accumulate(begin, end, 0.0f);
}

// Divide data into 4 parts
// Each separate part to process each subset
double add_parallel(std::vector<double> &vec) {
    auto vec0 = &vec[0];
    auto v_size = vec.size();
    
    // Starting threads, the subsets' range as argument to task function
    auto fut_1 = std::async(std::launch::async, sum_over, vec0, vec0 + v_size/4);
    auto fut_2 = std::async(std::launch::async, sum_over, vec0 + v_size/4, vec0 + 2 * v_size/4);
    auto fut_3 = std::async(std::launch::async, sum_over, vec0 + 2 * v_size/4, vec0 + 3 * v_size/4);
    auto fut_4 = std::async(std::launch::async, sum_over, vec0 + 3 * v_size/4, vec0 + v_size);

    return fut_1.get() + fut_2.get() + fut_3.get() + fut_4.get();

}

int main() {
    int n;
    std::cin >> n;

    std::vector<double> vec(n);
    std::iota(vec.begin(), vec.end(), 1.0);

    std::vector<double> vrand(10,000);
    std::generate(vrand.begin(), vrand.end(), [&vrand]() {
        return dist(mt);
    });

    std::cout << "Sum of first " << n << " integers: " << add_parallel(vec) << '\n';
    std::cout << "Sum of 10,000 random numbers in [0,1]: " << add_parallel(vrand) << '\n'; 
}