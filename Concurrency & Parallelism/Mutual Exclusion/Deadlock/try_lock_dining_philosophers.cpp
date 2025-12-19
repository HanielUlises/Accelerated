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

void print(int n, const std::string &str, int l_fork, int r_fork) {
    std::lock_guard<std::mutex> print_lock(print_mutex);
    std::cout << "Philosopher " << names[n] << str << l_fork << " and " << r_fork << '\n';
}

void print(int n, const std::string &str) {
    std::lock_guard<std::mutex> print_lock(print_mutex);
    std::cout << "Philosopher " << names[n] << str << std::endl; 
}

void dine(int n_philo) {
    int l_fork = n_philo;
    int r_fork = (n_philo + 1) % n_forks;
    
    print(n_philo, " forks are ", l_fork, r_fork);
    print(n_philo, " is thinking");

    std::this_thread::sleep_for(think_time);

    // Make an attempt to eat
    print(n_philo, " reaches for forks ", l_fork, r_fork);

    // Try to pick up both forks
    std::lock(fork_mutex[l_fork], fork_mutex[r_fork]);
    print(n_philo, " picks up fork ", l_fork, r_fork);

    // Current philosopher can now eat
    print(n_philo, " is eating...");
    ++mouthfuls[n_philo];

    std::this_thread::sleep_for(eat_time);

    print(n_philo, " puts down fork ", l_fork, r_fork);
    print(n_philo, " is thinking...");

    fork_mutex[l_fork].unlock();
    fork_mutex[r_fork].unlock();

    std::this_thread::sleep_for(think_time);
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