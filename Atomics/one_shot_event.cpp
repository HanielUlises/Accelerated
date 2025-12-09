#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>

std::atomic<bool> fired{false};
std::atomic<bool> waiting{false};

void signaler() {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(200ms);

    std::cout << "Signaler firing event\n";
    fired.store(true, std::memory_order_release);

    // If someone is waiting, poke them
    if (waiting.exchange(false, std::memory_order_acq_rel)) {
        fired.notify_one();
    }
}

void waiter(int id) {
    std::cout << "Thread " << id << " waiting...\n";

    if (!fired.load(std::memory_order_acquire)) {
        waiting.store(true, std::memory_order_release);

        // Recheck after publishing “I am waiting”
        if (!fired.load(std::memory_order_acquire)) {
            fired.wait(false);  // waits until becomes true
        }
    }

    std::cout << "Thread " << id << " resumed\n";
}

int main() {
    std::thread t1(waiter, 1);
    std::thread t2(signaler);

    t1.join();
    t2.join();
}
