#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#define LAMBDA_FUNC
// #define NORMAL_FUNC

typedef unsigned int ui;

bool action = true;

// We visualize the problems of scheduling, that is,
// the assignment of tasks, to see how one thread starts
// before another.
void log_actions(std::string name) {
    ui count = 0;
    while (action) {
        count++;
    }
    std::cout << name << " performs " << count << " actions" << std::endl;
}

int main() {
    // We can also define a lambda function, which is useful if
    // we want a specific method to be executed by a particular thread.
    auto actions_lambda = [](std::string name) {
        ui count = 0;
        while (action) {
            count++;
        }
        std::cout << name << " performs " << count << " actions" << std::endl;
    };

    // When creating a thread object, we pass to the constructor a function along
    // with the corresponding arguments to invoke that function.
    #ifdef NORMAL_FUNC
        std::thread agent_1(log_actions, "Agent 1");
        std::thread agent_2(log_actions, "Agent 2");
    #else
        std::thread agent_1(actions_lambda, "Agent 1");
        std::thread agent_2(actions_lambda, "Agent 2");
    #endif

    std::cout << "The agents are performing actions" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    action = false;
    agent_1.join();
    agent_2.join();
}