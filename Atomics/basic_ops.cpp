#include <iostream>
#include <atomic>

int main () {
    std::atomic<int> x = 0;
    std::cout << "After initialization: x = " << x << '\n'; ;
    x = 2;

    int y = x;
    std::cout << "After assignment: x = " << x << ", y = " << y << '\n'; 

    x.store(3);
    std::cout << "After store: x = " << x.load() << '\n'; 

    std::cout << "Exchange returns " << x.exchange(y) << '\n';
    std::cout << "After exchange: x = " << x << ", y = " << y << '\n';  
}
