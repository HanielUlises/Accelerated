#include <thread>
#include <mutex>
#include <iostream>

unsigned int first_item_count = 0;
unsigned int second_item_count = 0;

std::recursive_mutex mut;

void add_item_1(){
    mut.lock();
    first_item_count++;
    mut.lock();
}

void add_item_2(){
    mut.lock();
    second_item_count++;
    mut.lock();
}

void modifier(){
    for(int i = 0; i < 100000; i++){
        add_item_1();
        add_item_2();
    }
}

int main (){
    std::thread agent_1 (modifier);
    std::thread agent_2 (modifier);

    agent_1.join();
    agent_2.join();

    std::cout << "First item count \n" << first_item_count;
    std::cout << "Second item count \n" << second_item_count;
}