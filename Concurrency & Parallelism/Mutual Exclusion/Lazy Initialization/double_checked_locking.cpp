#include <mutex>
#include <thread>
#include <iostream>

class Test{
    public: 
        Test() {
            std::cout << "Test constructor called\n";
        }

        void func() {
            //
        }
};

Test *p_test = nullptr;
std::once_flag ptest_flag;

void process() {
    // Pass a callable object which performs initialization
    std::call_once(ptest_flag, []() {
        p_test = new Test;
    });
    p_test -> func();
}

int main() {
    std::thread thread_1 (process);
    std::thread thread_2 (process);

    thread_1.join(); thread_2.join();
}