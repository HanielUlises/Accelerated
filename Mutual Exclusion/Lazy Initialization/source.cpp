#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <memory>

class Test;

// Global pointer and mutex (could also be wrapped in a class)
static Test* pInstance = nullptr;
static std::mutex instanceMutex;

static std::once_flag testInitFlag;

// The actual class we want to lazily initialize
class Test {
private:
    int data;
    std::string name;

public:
    Test() : data(42), name("LazyInitializedTest") {
        std::cout << "Test constructor called! (Thread: "
                  << std::this_thread::get_id() << ")\n";
    }

    void func() const {
        std::cout << "Test::func() called - data = " << data
                  << ", name = " << name
                  << " (Thread: " << std::this_thread::get_id() << ")\n";
    }

    void doSomethingHeavy() const {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::cout << "Doing heavy work...\n";
    }

    ~Test() {
        std::cout << "Test destructor called.\n";
    }
};

// === Version 1: Double-Checked Locking Pattern (DCLP) ===
Test* getTestInstance_DCLP() {
    if (!pInstance) {  // First check (no lock)
        std::lock_guard<std::mutex> lock(instanceMutex);  // Lock
        if (!pInstance) {  // Second check (with lock)
            pInstance = new Test();  // Only created once
        }
    }
    return pInstance;
}

// === Version 2: std::call_once (Preferred in modern C++) ===
Test* getTestInstance_CallOnce() {
    std::call_once(testInitFlag, []() {
        pInstance = new Test();
    });
    return pInstance;
}

// === Version 3: Meyers' Singleton + Local Static (Best & Thread-Safe since C++11) ===
// This is usually the BEST way - no manual mutex needed!
class TestSingleton {
public:
    static Test& getInstance() {
        static Test instance;  
        return instance;
    }

    // Delete copy/move
    TestSingleton(const TestSingleton&) = delete;
    TestSingleton& operator=(const TestSingleton&) = delete;

private:
    TestSingleton() { std::cout << "Meyers' Singleton constructed!\n"; }
};

// === Version 4: Using std::unique_ptr with custom deleter for auto cleanup ===
static std::unique_ptr<Test> pSmartInstance;
static std::mutex smartMutex;

Test* getTestInstance_Smart() {
    if (!pSmartInstance) {
        std::lock_guard<std::mutex> lock(smartMutex);
        if (!pSmartInstance) {
            pSmartInstance = std::make_unique<Test>();
        }
    }
    return pSmartInstance.get();
}

// Concurrent access
void worker(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50 + id * 10));

    std::cout << "Worker " << id << " starting...\n";

    // Lazy initialization methods
    Test* t1 = getTestInstance_DCLP();
    t1->func();

    Test* t2 = getTestInstance_CallOnce();
    t2->doSomethingHeavy();

    Test& t3 = TestSingleton::getInstance();
    t3.func();

    Test* t4 = getTestInstance_Smart();
    t4->func();

    std::cout << "Worker " << id << " finished.\n";
}

int main() {
    std::cout << "=== Thread-Safe Lazy Initialization Demo ===\n\n";

    const int numThreads = 10;
    std::vector<std::thread> threads;

    // Launch multiple threads that all try to access the lazy instance
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\nAll threads completed. Only one Test object was created!\n";

    // Demonstrate Meyers' singleton is still alive
    TestSingleton::getInstance().func();

    return 0;
}