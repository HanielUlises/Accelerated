#include <coroutine>
#include <iostream>

struct Generator{
        struct promise_type{
            int current_value;

            std::suspend_always yield_value(int value){
                current_value = value;
                return {};
            }

            std::suspend_always initial_suspend() noexcept { return {};}
            std::suspend_always final_suspend() noexcept { return {};}
            Generator get_return_object() {return Generator{*this};}
            void unhandled_exception() {}
            void return_void() {}
        };

        using handle_type = std::coroutine_handle<promise_type>;

        Generator(promise_type &p) : coro(handle_type::from_promise(p)){}
        ~Generator(){
            if(coro) coro.destroy();
        }

        bool next() {return coro ? (coro.resume(), !coro.done()) : false;}
        int value() {return coro.promise().current_value;}

    private:
        handle_type coro;
};

Generator count_to (int limit){
    for(int i = 1; i <= limit; i++) co_yield i;
}

int main() {
    auto gen = count_to(5);
    while(gen.next()) std::cout << gen.value() << " ";
    std::cout << std::endl;
}