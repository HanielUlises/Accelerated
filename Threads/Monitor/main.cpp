#include <iostream>
#include <thread>
#include <chrono>

#include "monitor.h"

class ThreadSafeCounter : public Monitor {
private:
    int value_ = 0;
    const int threshold_ = 10;

public:
    void increment() {
        lock();
        ++value_;
        std::cout << "Incremented to " << value_ << '\n';
        if (value_ >= threshold_) {
            notify_all();
        }
        unlock();
    }

    // Wait until counter reaches or exceeds threshold
    void wait_for_threshold() {
        wait([this] { return value_ >= threshold_; });
        std::cout << "Threshold reached! Value is " << value_ << '\n';
    }

    int get_value() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return value_;
    }
};

int main() {
    ThreadSafeCounter counter;

    // Thread that waits for the threshold
    std::thread waiter([&counter] {
        std::cout << "Waiter starting...\n";
        counter.wait_for_threshold();
        std::cout << "Waiter done.\n";
    });

    // Threads that increment the counter
    auto incrementer = [&counter] {
        for (int i = 0; i < 5; ++i) {
            counter.increment();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    };

    std::thread inc1(incrementer);
    std::thread inc2(incrementer);

    inc1.join();
    inc2.join();
    waiter.join();

    std::cout << "Final value: " << counter.get_value() << '\n';
}