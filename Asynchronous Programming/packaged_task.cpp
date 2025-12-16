#include <future>
#include <iostream>
#include <chrono>

using namespace std::literals;

int main() {
    std::packaged_task<int(int, int)> p_task ([](int a, int b) {
        std::this_thread::sleep_for(2s);
        
        return a + b;
    });

    std::future<int> fut = p_task.get_future();
    int a, b;
    std::cin >> a >> b;
    std::thread thread_1(std::move(p_task), a, b);

    auto display_result = [a, b](std::future<int> &&fut_obj) {
        std::cout << "Waiting for result..." << '\n';
        std::cout << a << " + " << b << " = " << fut_obj.get() << '\n';
    };
    
    std::thread thread_2(display_result, std::move(fut));

    thread_1.join(); thread_2.join();
}