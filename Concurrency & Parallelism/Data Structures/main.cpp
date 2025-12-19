#include <iostream>
#include <future>
#include <string>
#include <thread>

#include "concurrent_queue.h"

concurrent_queue<std::string> conc_queue;

void reader() {

    std::string s_data;

    std::cout << "Reader popping elements" << '\n';
    for(int i = 0; i < 50; i++) {
        conc_queue.pop(s_data);
        std::cout << "Buffering data: " << s_data << '\n';
    }
}

void writer() {
    using namespace std::literals;

    std::this_thread::sleep_for(2s);

    std::cout << "Writer pushing elements..." << '\n';
    for(int i = 0; i < 60; i++) {
        std::string s_data = "Item " + std::to_string(i);
        conc_queue.push(s_data);
    }

    std::cout << "Writer returned from push... " << '\n';
}

int main() {
    auto write_fut = std::async(std::launch::async, writer);
    auto read_fut = std::async(std::launch::async, reader);

    try {
        read_fut.get();
    } catch (std::exception &e) {
        std::cout << "Exception caught: " << e.what() << '\n';
    }

    try {
        write_fut.get();
    } catch (std::exception &e) {
        std::cout << "Exception caught: " << e.what() << '\n';
    }

}