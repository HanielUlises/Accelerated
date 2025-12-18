#include <algorithm>
#include <execution>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v (20000);
    int counter = 0;

    std::cout << "=======================" << '\n';
    std::cout << "Count positions" << '\n';
    std::for_each(std::execution::par_unseq, v.begin(), v.end(), [&counter](int &n) {
        n = counter++;
    });

    for(auto i : v) 
        std::cout << i << ' ';
    std::cout << '\n';

    std::vector<int> v1 = {1,2,3,4};
    std::vector<int> v2 (v1.size());

    std::cout << "=======================" << '\n';
    std::cout << "Exclusive scan" << '\n';
    std::cout << "Original Vector \n";

    for (const auto &i: v1) std::cout << i << " ";
    std::cout << '\n';
    std::exclusive_scan(std::execution::par_unseq, v1.begin(), v1.end(), v2.begin(),-1);

    std::cout << "After exclusive scan on -1: \n";
    for(const auto &i: v2) std::cout << i << " ";
    std::cout << '\n';

    // Map and reduce
    std::cout << "=======================" << '\n';
    std::vector<int> x = {1,2,3,4,5};
    std::vector<int> y = {5,4,3,2,1};

    auto result = std::transform_reduce(std::execution::par, x.begin(), x.end(), y.begin(), 0);

    std::cout << "First vector: \n";
    for(const auto &i : x) std::cout << i << ' ';
    std::cout << '\n'; 

    std::cout << "Second vector: \n";
    for(const auto &i : y) std::cout << i << ' ';
    std::cout << '\n'; 

    std::cout << "Result from transform op (default): "<< result << '\n';
}