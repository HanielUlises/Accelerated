#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string> 
#include <chrono>

using namespace std::literals;

constexpr int n_forks = 5;
constexpr int n_philosophers = n_forks;
std::string names[n_philosophers] = {"A", "B", "C", "D", "E"};

// How many times a philosopher is able to eat
int mouthfuls[n_philosophers] = {0};

// A philosopher is thinking when they have not picked both forks
constexpr std::chrono::seconds think_time = 2s;

// A philosopher is eating when the have picked both forks
constexpr std::chrono::seconds eat_time = 1s;

// A mutex avoids more than two philosophers picking the same fork
// A philosopher thread can only pick up a fork if it can lock the corresponding mutex
std::mutex fork_mutex[n_forks];

// Mutex to protect the output;
std::mutex print_mutex;

void print(int n, const std::string &str, int fork_number) {
    std::lock_guard<std::mutex> print_lock(print_mutex);
    std::cout << "Philosopher " << names[n] << str << fork_number << std::endl; 
}

void print(int n, const std::string &str) {
    std::lock_guard<std::mutex> print_lock(print_mutex);
    std::cout << "Philosopher " << names[n] << str << std::endl; 
}

void dine(int n_philo) {
    int left  = n_philo;
    int right = (n_philo + 1) % n_forks;

    while (true) {
        // Try to pick left fork
        if (fork_mutex[left].try_lock()) {
            print(n_philo, " picked left fork ", left);

            // Got left fork, now try right fork
            if (fork_mutex[right].try_lock()) {
                // Both forks acquired, eat once and exit
                print(n_philo, " picked right fork ", right);
                print(n_philo, " is eating");

                std::this_thread::sleep_for(eat_time);
                mouthfuls[n_philo]++;

                print(n_philo, " puts down forks");
                fork_mutex[right].unlock();
                fork_mutex[left].unlock();
                return;                   
            }
            else {
                // Couldn’t get right fork, put left one back immediately
                print(n_philo, " couldn’t get right fork, releasing left");
                fork_mutex[left].unlock();

                // Be "polite": tiny pause before retrying
                std::this_thread::sleep_for(10ms);
            }
        }
        else {
            // Couldn’t even get left fork, tiny pause and retry
            std::this_thread::sleep_for(10ms);
        }
    }
}

int main() {
    std::vector<std::thread> philos;

    for(int i = 0; i < n_philosophers; i++) {
        philos.push_back(std::move(std::thread{dine, i}));
    }

    for(auto &philo : philos) {
        philo.join();
    }

    for(int i = 0; i < n_philosophers; i++) {
        std::cout << "Philosopher " << names[i]; 
        std::cout << " had " << mouthfuls[i] << " mouthfuls \n";
    }

    return 0;
}