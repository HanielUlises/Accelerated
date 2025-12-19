#include <iostream>
#include <thread>
#include <condition_variable>
#include <string>
#include <chrono>

using namespace std::literals;

std::string s_data;

std::mutex mut;
std::condition_variable cond_var;

void reader() {
    std::cout << "Reader thread locking mutex." << std::endl;
    std::unique_lock<std::mutex> unique_lock(mut);
    std::cout << "Reader thread has locked the mutex." << std::endl;

    std::cout << "Reader thread is sleeping." << std::endl;
    cond_var.wait(unique_lock);

    std::cout << "Reader thread \"wakes up\" " << std::endl;
    std::cout << "Data is " << s_data << std::endl;
}

void writer() {
    {
        std::cout << "Writer thread locking the mutex..." << std::endl;
        std::lock_guard<std::mutex> lock_guard(mut);
        std::cout << "Writer thread has locked the mutex." << std::endl;

        std::this_thread::sleep_for(2s);
        
        std::cout << "Writer thread modifying data..." << std::endl;
        s_data = "populated";
    }

    std::cout << "Writer thread sends notification." << std::endl;
    cond_var.notify_one();
}

int main() {
    s_data = "empty";

    std::cout << "Data is \"" << s_data << "\"" << std::endl;

    std::thread read(reader);
    std::thread write(writer);
    
    read.join(); write.join();

    return 0;
}