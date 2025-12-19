#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

#define USE_EMPLACE_BACK
// #define USE_PUSH_BACK

// A **mutex** (mutual exclusion) is used to ensure that only one thread at a time can
// access a shared resource, such as a variable or data structure.
// This is important because, without protection, multiple threads could attempt to modify
// the same resource simultaneously.

std::mutex mtx;

// Test structure.
// We will modify the field
// given a delta step.
struct Data {
    Data(int v) : value(v) {}
    int value;

    void modify(int delta) {
        value += delta;
    }
};

// Delta as increment
void test_thread(std::shared_ptr<Data> shared_data, int delta) {
    // Here we use **std::lock_guard**, which simplifies mutex handling. When creating a
    // lock_guard object and passing it a mutex, the mutex is automatically locked.
    // This means the mutex remains locked while the lock_guard is in scope.
    // When the lock_guard goes out of scope, either because the function ends or an
    // exception occurs, the mutex is automatically released. This helps prevent
    // forgetting to unlock and potential program deadlocks, making our code safer
    // and easier to maintain.
    std::lock_guard<std::mutex> lock(mtx);
    // Thread starts
    std::cout << "---------------------------------------------------\n";
    std::cout << "[Thread " << std::this_thread::get_id() << "] Starting with delta = " << delta << std::endl;

    // Modify the value
    shared_data->modify(delta);
    std::cout << "[Thread " << std::this_thread::get_id() << "] Modified value to " << shared_data->value << std::endl;

    // Thread ends
    std::cout << "[Thread " << std::this_thread::get_id() << "] Finished." << std::endl;
    std::cout << "---------------------------------------------------\n";
}

int main() {
    auto start_time = std::chrono::steady_clock::now();

    // To avoid using raw pointers,
    // we prefer to use a shared pointer.
    // Try using a unique pointer instead, and you will see the error.
    // std::unique_ptr<Data> p1 = std::make_unique<Data>(10);
    std::shared_ptr<Data> p1 = std::make_shared<Data>(10);
    std::vector<std::thread> threads;

    #ifdef USE_EMPLACE_BACK
        std::cout << "Using emplace_back to add threads." << std::endl;
    #else
        std::cout << "Using push_back to add threads." << std::endl;
    #endif

    // emplace_back is more versatile than push_back; in short,
    // push_back creates an object outside the vector, while emplace_back
    // creates the object inside it.
    for (int i = 0; i < 5; i++) {
        // Essentially, it’s like a constructor for the type of object to be used.
        // "threads" is a type of object <thread>, which takes in its constructor
        // a function (in this case, the test function) and its subsequent arguments.
        // For a function with n arguments, the number of arguments for the
        // constructor of a thread object will be n+1.
        #ifdef USE_EMPLACE_BACK
            threads.emplace_back(test_thread, p1, i + 1);
        #else
            threads.push_back(std::thread(test_thread, p1, i + 1));
        #endif
    }

    for (auto &thread : threads) {
        thread.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    std::cout << "Final value: " << p1->value << std::endl;
    std::cout << "Total execution time: " << total_time << "ms" << std::endl;
}