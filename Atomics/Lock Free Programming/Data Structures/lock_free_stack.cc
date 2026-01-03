#include <atomic>
#include <utility>

template<typename T>
class LockFreeStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
        Node(T&& d) : data(std::move(d)), next(nullptr) {}
    };

    std::atomic<Node*> head;

public:
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