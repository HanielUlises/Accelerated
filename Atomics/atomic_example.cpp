// Two shoppers adding items to a shared notepad

#include <thread>
#include <atomic>    
#include <iostream>

std::atomic<unsigned int> item_count (0);

std::mutex pencil;

void shopper(){
    pencil.lock();
    for(int i = 0; i < 10000000; i++){

        item_count++;
    }
    pencil.unlock();
}

int main (int argc, char *argv[]){
    std::thread agent_1 (shopper);
    std::thread agent_2 (shopper);

    agent_1.join();
    agent_2.join();
    std::cout << "We should get " << item_count << " items \n";
}