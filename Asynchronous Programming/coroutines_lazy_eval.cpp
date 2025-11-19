#include <iostream>
#include <coroutine>

struct LazyEvalue{
    struct promise_type{
        int value;
        std::suspend_always initial_suspend() {return {};}
        std::suspend_always final_suspend () noexcept {return {};}
        LazyEvalue get_return_object() {return LazyEvalue{std::coroutine_handle<promise_type>::from_promise(*this)};}
        std::suspend_always yield_value(int v) {
            value = v;
            return {};
        }

        void return_void() {}
        void unhandled_exception() {}

    }; 

    using handle_type = std::coroutine_handle<promise_type>;
    handle_type coroutine;

    LazyEvalue(handle_type h) : coroutine(h) {}
    ~LazyEvalue() {if(coroutine) coroutine.destroy();}

    int get(){
        coroutine.resume();
        return coroutine.promise().value;
    }
};

LazyEvalue compute_value() {
    std::cout << "Computing value..." << std::endl;
    co_yield 42;
}

int main() {
    LazyEvalue lazy_evalue = compute_value();

    std::cout << "Value is needed... " << std::endl;
    int value = lazy_evalue.get();
    std::cout << "Value: " << value << std::endl;
    
    return 0;
}