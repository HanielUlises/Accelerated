#include <future>
#include <iostream>
#include <thread>
#include <chrono>

std::mutex mut;

void produce(std::promise<int> &px) {
    using namespace std::chrono_literals;

    int x = 42;
    std::this_thread::sleep_for(2s);

    std::cout << "Promise sets shared state to " << x << std::endl;
    px.set_value(x);
}

void consume(std::shared_future<int> &fx) {
    std::cout << "Thread " << std::this_thread::get_id() << " calling get()..." << std::endl;
    
    int x = fx.get();

    std::lock_guard<std::mutex> lock_guard(mut);

    std::cout << "Thread " << std::this_thread::get_id() << " returns from calling get()..." << std::endl;
    std::cout << "Thread " << std::this_thread::get_id() << " has value: " << x;
}

int main() {
    std::promise<int> promise_;
    std::shared_future<int> shared_fut = promise_.get_future();
    std::shared_future<int> shared_fut_copy = shared_fut;

    std::thread thread_producer(produce, std::ref(promise_));

    std::thread consoomer_1(consume, std::ref(shared_fut));
    std::thread consoomer_2(consume, std::ref(shared_fut_copy));

    thread_producer.join(); consoomer_1.join(); consoomer_2.join();
}