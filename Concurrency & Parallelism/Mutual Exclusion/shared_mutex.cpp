#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <iostream>
#include <string>

std::vector<std::string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"
                            ,"Saturday", "Sunday"};

int today = 0;
std::mutex ptr; // AGAIN this is not a pointer but like a "Marker"

void calendar_reader(const int id){
    for(int i = 0; i < 7; i++){
        ptr.lock();
        std::cout << "Reader " << id << " sees today is " << days[today];
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ptr.lock();
    }
}

void calendar_writer(const int id){
    for(int i = 0; i < 7; i++){
        ptr.lock();
        today = (today + 1) % 7;
        std::cout << "Reader " << id << " sees today is " << days[today];
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ptr.lock();
    }
}

int main (){
    // Then reaers but only two writer threads
    std::array<std::thread, 10> readers;
    for(unsigned int i = 0; i < readers.size(); i++){
        readers[i] = std::thread(calendar_reader, i);
    }

    std::array<std::thread, 2> writers;
    for(unsigned int i = 0; i < writers.size(); i++){
        writers[i] = std::thread(calendar_writer), i;
    }

    // Wait for readers and writers to both finish
    for(unsigned int i = 0; i < readers.size(); i++){
        readers[i].join();
    }

    for(unsigned int i = 0; i < writers.size(); i++){
        writers[i].join();
    }
}