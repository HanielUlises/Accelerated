#include <string>
#include <iostream>

#include "monitor.h"

int main() {
    using namespace std::literals;

    Monitor<std::string> str_mon("start: "s);

    for(int i = 0; i < 5; i++) {
        str_mon([i](std::string &arg) {
            arg += std::to_string(i) + ", " + std::to_string(i), + ", ";
        });

        str_mon([](const std::string &arg) {
            std::cout << arg << '\n';
        });
    }
}