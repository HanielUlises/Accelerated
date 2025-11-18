#include <coroutine>
#include <iostream>
#include <optional>

template<typename T>
struct Generator {
    struct promise_type {
        T value;
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        std::suspend_always yield_value(T val) { value = val; return {}; }
        Generator get_return_object() { return Generator{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        void return_void() {}
    };

    std::coroutine_handle<promise_type> h;

    explicit Generator(std::coroutine_handle<promise_type> handle) : h(handle) {}
    ~Generator() { if (h) h.destroy(); }

    bool next() {
        if (h && !h.done()) {
            h.resume();
            return !h.done();
        }
        return false;
    }

    T value() const { return h.promise().value; }
};

// Example coroutine
Generator<int> countdown(int from) {
    for (int i = from; i > 0; --i) {
        co_yield i;
        std::this_thread::sleep_for(500ms);
    }
}

int main() {
    auto gen = countdown(5);
    while (gen.next()) {
        std::cout << "Count: " << gen.value() << "\n";
    }
}