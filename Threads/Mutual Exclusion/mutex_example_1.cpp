// Two shoppers adding items to a shared notepad

#include <thread>
#include <mutex>    

unsigned int item_count = 0;
std::mutex pencil;

void shopper(){
    pencil.lock();
    for(int i = 0; i < 10000000; i++){
        std::cout << "Shopper " << std::this_thread::get_id() << " is thinking... \n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
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