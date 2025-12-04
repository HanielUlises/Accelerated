#include <iostream>
#include <thread>
#include <condition_variable>
#include <string>

using namespace std::literals;

std::string s_data;

std::mutex mut;

std::condition_variable cond_var;

bool condition = false;

void reader() {
    std::cout << "Reader thread locking mutex." << std::endl;
    std::unique_lock<std::mutex> unique_lock(mut);
    std::cout << "Reader thread has locked the mutex." << std::endl;

    std::cout << "Reader thread is sleeping." << std::endl;
    cond_var.wait(unique_lock, [](){return condition;});

    std::cout << "Reader thread" << std::this_thread::get_id() << " \"wakes up\" " << std::endl;
    std::cout << "Data is " << s_data << std::endl;
    std::cout << "Reader thread unlocks mutex" << std::endl;
}

void writer() {
    {
        std::cout << "Writer thread locking the mutex..." << std::endl;
        std::lock_guard<std::mutex> lock_guard(mut);
        std::cout << "Writer thread has locked the mutex." << std::endl;

        std::this_thread::sleep_for(2s);

        std::cout << "Writer thread modifying data..." << std::endl;
        s_data = "modified";

        condition = true;

        std::cout << "Writer thread unlocks the mutex." << std::endl;
    }

    std::cout << "Writer thread sends notification." << std::endl;
    cond_var.notify_all();
}
 
int main() {
    s_data = "emtpy";

    std::cout << "Data is " << s_data << std::endl;

    std::thread write(writer);
    std::this_thread::sleep_for(10ms);
    std::thread read_1(reader);
    std::this_thread::sleep_for(10ms);
    std::thread read_2(reader);
    std::this_thread::sleep_for(10ms);
    std::thread read_3(reader);
    std::this_thread::sleep_for(10ms);

    write.join(); read_1.join(); read_2.join(); read_3.join();

    return 0;
}