#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <random>

class BankAccount {
private:
    int balance;
    std::mutex mtx;  // Mutex to protect the balance

public:
    BankAccount(int initial_balance) : balance(initial_balance) {}

    // Method to transfer money between accounts
    bool transfer(BankAccount &destination, int amount) {
        // Lock both mutexes to prevent race conditions
        std::unique_lock<std::mutex> lock_source(mtx, std::defer_lock);
        std::unique_lock<std::mutex> lock_destination(destination.mtx, std::defer_lock);

        // Avoid deadlocks
        std::lock(lock_source, lock_destination); 

        if (balance >= amount) {
            balance -= amount;
            destination.balance += amount;
            std::cout << "Transfer of " << amount << " completed.\n";
            return true;
        } else {
            std::cout << "Insufficient balance to transfer " << amount << ".\n";
            return false;
        }
    }

    int getBalance() const {
        return balance;
    }
};

void Transfer(BankAccount &account1, BankAccount &account2, int num_transfers) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);

    for (int i = 0; i < num_transfers; ++i) {
        int amount = dist(gen);
        if (!account1.transfer(account2, amount)) {
            // If transfer from account1 to account2 fails, try the reverse
            account2.transfer(account1, amount);
        } 
        // Pause for (simulated) processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    BankAccount account1(1000);
    BankAccount account2(1000);

    // Start multiple threads for concurrent transfers
    std::thread t1(Transfer, std::ref(account1), std::ref(account2), 10);
    std::thread t2(Transfer, std::ref(account2), std::ref(account1), 10);
    std::thread t3(Transfer, std::ref(account1), std::ref(account2), 10);
    std::thread t4(Transfer, std::ref(account2), std::ref(account1), 10);

    // Wait for all threads to finish
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << "Final balance account 1: " << account1.getBalance() << "\n";
    std::cout << "Final balance account 2: " << account2.getBalance() << "\n";

    return 0;
}