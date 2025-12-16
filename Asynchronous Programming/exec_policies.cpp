#include <algorithm>
#include <execution>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v (20,000);
    int counter = 0;

    std::for_each(std::execution::par, v.begin(), v.end(), [&counter](int n) {
        n = counter++;
    });

    for(auto i : v) 
        std::cout << i << ' ';
    std::cout << '\n';
}