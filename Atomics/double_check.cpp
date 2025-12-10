#include <mutex>
#include <atomic>
#include <memory> 

class some_type {
    // ...
public:
    void do_it() { /*...*/ }
};

std::atomic<std::shared_ptr<some_type>> ptr{nullptr}; 
std::mutex process_mutex;

void process() {
    auto temp = ptr.load(std::memory_order_acquire);
    if (!temp) { 
        std::lock_guard<std::mutex> lk(process_mutex);
        
        temp = ptr.load(std::memory_order_acquire);
        if (!temp) {
            temp = std::make_shared<some_type>();
            ptr.store(temp, std::memory_order_release);
        }
    }
    temp->do_it();
}
