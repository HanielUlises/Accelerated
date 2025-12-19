#include <thread>
#include <iostream>
#include <string>
#include <format>

class thread_guard{
        std::thread thr;

    public:
        explicit thread_guard(std::thread &&thr) : thr(std::move(thr)) {}

        thread_guard(const thread_guard &) = delete;
        thread_guard& operator=(const thread_guard &) = delete;

        ~thread_guard() {
            if(thr.joinable())
                thr.join();
        }
};

constexpr void print(const std::string_view str_fmt, auto&&... args){
    fputs(std::vformat(str_fmt, std::make_format_args(args...)).c_str(), stdout);
}

int main() {

    try{
        std::thread thr([] () {int x = 5; print("On the intercom Rosie told me to come {}", x);});
        throw std::exception();
    }catch(std::exception &e){
        std::cout << "Exception caught " << e.what() << std::endl; 
    }

}