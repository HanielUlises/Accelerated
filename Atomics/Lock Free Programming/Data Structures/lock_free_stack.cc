#include <atomic>
#include <thread>
#include <vector>
#include <iostream>

template<typename T>
struct LockFreeStack {
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
        Node(T&& d) : data(std::move(d)), next(nullptr) {}
    };

    std::atomic<Node*> head;

    LockFreeStack() : head(nullptr) {}
    ~LockFreeStack() {
        Node* curr = head.load();
        while (curr) {
            Node* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
    }

    void push(const T& value) {
        Node* newNode = new Node(value);
        Node* oldHead;
        do {
            oldHead = head.load();
            newNode->next = oldHead;
        } while (!head.compare_exchange_weak(oldHead, newNode));
    }

    void push(T&& value) {
        Node* newNode = new Node(std::move(value));
        Node* oldHead;
        do {
            oldHead = head.load();
            newNode->next = oldHead;
        } while (!head.compare_exchange_weak(oldHead, newNode));
    }
};

int main() {
    LockFreeStack<int> stack;

    const int num_threads = 8;
    const int pushes_per_thread = 100000;

    auto worker = [&stack]() {
        for (int i = 0; i < pushes_per_thread; ++i) {
            stack.push(i);
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }

    int count = 0;
    for (auto* node = stack.head.load(); node != nullptr; node = node->next) {
        ++count;
    }

    std::cout << "Total elements pushed: " << num_threads * pushes_per_thread << std::endl;
    std::cout << "Elements in stack: " << count << std::endl;

    return 0;
}