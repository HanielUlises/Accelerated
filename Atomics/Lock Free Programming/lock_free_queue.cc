#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <iostream>

template <typename T>
class LockFreeQueue {
private:
    struct Node {
        T data;
        std::atomic<Node*> next{nullptr};
        Node(const T& val) : data(val) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node(T{});  // Sentinel node
        head.store(dummy);
        tail.store(dummy);
    }

    ~LockFreeQueue() {
        T tmp;
        while (consume(tmp)) {}  // Drain queue
        delete head.load();
    }

    void produce(const T& value) {
        Node* new_node = new Node(value);
        Node* old_tail;
        Node* next;

        while (true) {
            old_tail = tail.load(std::memory_order_acquire);
            next = old_tail->next.load(std::memory_order_acquire);

            if (old_tail == tail.load(std::memory_order_acquire)) {  
                if (next == nullptr) {  
                    if (old_tail->next.compare_exchange_weak(next, new_node,
                                                             std::memory_order_release,
                                                             std::memory_order_relaxed)) {
                        tail.compare_exchange_weak(old_tail, new_node,
                                                  std::memory_order_release,
                                                  std::memory_order_relaxed);
                        return;
                    }
                } else {
                    // Help advance tail
                    tail.compare_exchange_weak(old_tail, next,
                                              std::memory_order_release,
                                              std::memory_order_relaxed);
                }
            }
        }
    }

    bool consume(T& result) {
        Node* old_head;
        Node* old_tail;
        Node* next;

        while (true) {
            old_head = head.load(std::memory_order_acquire);
            old_tail = tail.load(std::memory_order_acquire);
            next = old_head->next.load(std::memory_order_acquire);

            if (old_head == head.load(std::memory_order_acquire)) {
                if (old_head == old_tail) {  // Empty?
                    if (next == nullptr) {
                        return false;
                    }
                    // Help advance tail
                    tail.compare_exchange_weak(old_tail, next,
                                              std::memory_order_release,
                                              std::memory_order_relaxed);
                } else {
                    result = next->data;  // Read before swinging head
                    if (head.compare_exchange_weak(old_head, next,
                                                   std::memory_order_release,
                                                   std::memory_order_relaxed)) {
                        delete old_head;  // Safe to delete old dummy/head
                        return true;
                    }
                }
            }
        }
    }
};

int main() {
    LockFreeQueue<int> lfq;

    std::vector<std::thread> threads;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(&LockFreeQueue<int>::produce, &lfq, i);
    }

    int consumed = 0;
    for (int i = 0; i < 10; ++i) {  // 10 consumers to drain
        threads.emplace_back([&lfq, &consumed]() {
            int val;
            while (lfq.consume(val)) {
                ++consumed;
                std::cout << val << " ";
            }
        });
    }

    for (auto& t : threads) t.join();

    std::cout << "\nConsumed: " << consumed << "\n";
}