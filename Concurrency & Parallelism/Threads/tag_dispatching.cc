#include <iostream>
#include <vector>
#include <execution>
#include <type_traits>

template <typename T>
struct use_parallel : std::bool_constant<std::is_trivially_copyable_v<T> && sizeof(T) <= 16> {};

template <typename Iter, typename Func>
void for_each_dispatch(Iter first, Iter last, Func func, std::true_type) {
    std::for_each(std::execution::par, first, last, func);
}

template <typename Iter, typename Func>
void for_each_dispatch(Iter first, Iter last, Func func, std::false_type) {
    std::for_each(first, last, func);
}

template <typename Iter, typename Func>
void parallel_for_each(Iter first, Iter last, Func func) {
    for_each_dispatch(first, last, func, use_parallel<typename std::iterator_traits<Iter>::value_type>{});
}

int main() {
    std::vector<double> data(1'000'000, 1.0);

    parallel_for_each(data.begin(), data.end(), [](double& x) {
        x *= 2.0;
    });

    std::cout << data[0] << '\n';
}