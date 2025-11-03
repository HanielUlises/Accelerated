#include <format>
#include <deque>
#include <thread>
#include <mutex>

// Producer-consumer idiom: one process produces data 
// and another consumes data using a container to hold the data


constexpr void print(const std::string_view str_fmt, auto&&... args){
    fputs(std::vformat(str_fmt, std::make_format_args(args...)).c_str(), stdout);
}

using namespace std::chrono_literals;

constexpr size_t num_items{10};
constexpr auto delay_time{205ms};


std::mutex p_mtx{}; // Producer mutex
std::mutex c_mtx{}; // Consumer mutex
std::deque<size_t> q{};
std::atomic_flag finished{};


void sleep_ms(const auto &delay){
    std::this_thread::sleep_for(delay);
}

void producer() {
    for(size_t i = 0; i < num_items; i++){
        print("Push {} on the queue\n", i);
        std::lock_guard<std::mutex> lck{p_mtx};
        q.push_back(i);
    }

    std::lock_guard<std::mutex> lck(p_mtx);
    finished.test_and_set();
}

void consumer() {
    while(!finished.test()){
        sleep_ms(delay_time * 3);
        std::lock_guard<std::mutex> lck {p_mtx};
        while(!q.empty()){
            print("Pop {} from the queue\n", q.front());
            q.pop_front();
        }
    }
}

auto main() -> int{
    std::thread t1 {producer};
    std::thread t2 {consumer};
    t1.join();
    t2.join();
    
    print("Finished\n");
}