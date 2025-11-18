#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <iostream>

unsigned int registered = 0;
std::mutex ptr_register; // THIS IS NOT a pointer but like a writer, i just wanted to add ptr to its name

void agent(const std::string name){
    int registered = 0;
    while (registered)    {
        if(registered){
            ptr_register.lock();
            registered += registered;
            std::cout << name << " wrote " << registered << " entries " << '\n';
        }else{
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            registered++;
            std::cout << name << " did something else " << '\n';
        }
    }
    
}

int main (){
    auto start_time = std::chrono::steady_clock::now();
    std::thread agent_1 (agent, "Agent 1");
    std::thread agent_2 (agent, "Agent 2");
    agent_1.join();
    agent_2.join();

    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_time).count();  
    std::cout << "Elapsed time: " << elapsed_time/1000.0;
}