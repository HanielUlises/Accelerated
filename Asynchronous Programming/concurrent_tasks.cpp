#include <algorithm>
#include <future>
#include <iostream>
#include <vector>
#include <thread>

int square(int x) {
    std::cout << "Squaring " << x << "in thread" << std::this_thread::get_id() << std::endl;
    return x * x;
}

int main() {
    std::vector<int> numbers = {1,2,3,4,5};
    std::vector<std::future<int>> futures;

    // Launch tasks concurrently using std::async
    std::transform(numbers.begin(), numbers.end(), std::back_inserter(futures), 
        [](int x){
            return std::async(std::launch::async, square, x);
        }
    );

    std::vector<int> results;

    // Gather results from futures

    std::transform(futures.begin(), futures.end(), std::back_inserter(results),
      [](std::future<int> &f){return f.get();}  
    );

    std::cout << "Results "<< std::endl;
    for(const auto &i: results)
        std::cout << i << " ";

    return 0;
}