#include <future>
#include <iostream>
#include <thread>
#include <exception>
#include <chrono>

void produce(std::promise<int> &px) {
    try {
        using namespace std::chrono_literals;
        int x = 42;
        std::this_thread::sleep_for(2s);

        if(1) {
            throw std::out_of_range("Out of range");
        }

        std::cout << "Promise sets shared to " << x << std::endl;
        px.set_value(x);
    } catch (...) {
        px.set_exception(std::current_exception());
    }
}

void consume(std::future<int> &fx) {
    std::cout << "Future calling get() ..." << std::endl;
    try {
        int x = fx.get();
        std::cout << "Future returns from calling method get()" << std::endl;
        std::cout << "The response is " << x << std::endl;
    } catch (std::exception &e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }
}

int main() {
    std::promise<int> promise_;
    std::future<int> future_ = promise_.get_future();

    std::thread thr_producer(produce, std::ref(promise_));
    std::thread thr_consumer(consume, std::ref(future_));

    thr_producer.join(); thr_consumer.join();

}