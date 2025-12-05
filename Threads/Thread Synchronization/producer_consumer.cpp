#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <string>

using namespace std::literals::chrono_literals;

// The Producer function
// It takes a std::promise object by reference. Its job is to compute a value
// and set that value into the promise.
void producer(std::promise<std::string>& p) {
    std::cout << "[Producer Thread]: Starting work (simulated 3s delay)..." << std::endl;

    std::this_thread::sleep_for(3s); 

    // 2. The result is ready
    std::string result = "The computed data packet is ready: ID-9001";
    std::cout << "[Producer Thread]: Work finished. Setting result into the promise." << std::endl;
    
    // 3. Fulfill the promise, making the associated future ready
    p.set_value(result);
    
    std::cout << "[Producer Thread]: Promise fulfilled and thread exiting." << std::endl;
}

// The Consumer function
// It takes a std::future object by reference. Its job is to wait for the value
// from the future and process it.
void consumer(std::future<std::string>& f) {
    std::cout << "[Consumer Thread]: Waiting for the future to become ready..." << std::endl;

    // 1. Wait for the result to be available (this call blocks until producer calls set_value)
    // The shared state linking the promise and future handles the synchronization.
    std::string data = f.get(); 

    // 2. Process the received data
    std::cout << "\n=================================================" << std::endl;
    std::cout << "[Consumer Thread]: Future is ready! Received data:" << std::endl;
    std::cout << "Data: \"" << data << "\"" << std::endl;
    std::cout << "=================================================\n" << std::endl;
}

int main() {
    // 1. Setup the synchronization channel: promise and future
    // The promise holds the result, and the future is used to retrieve it.
    std::promise<std::string> data_promise;
    std::future<std::string> data_future = data_promise.get_future();
    
    std::cout << "Main Thread: Synchronization objects created." << std::endl;

    // 2. Launch the producer thread, giving it the promise (sender role)
    // We pass the promise object to the thread to let it set the value.
    std::thread t_producer(producer, std::ref(data_promise));

    // 3. Launch the consumer thread, giving it the future (receiver role)
    // We pass the future object to the thread to let it wait and retrieve the value.
    std::thread t_consumer(consumer, std::ref(data_future));

    // 4. Wait for both threads to finish execution
    t_producer.join();
    t_consumer.join();

    std::cout << "Main Thread: All threads completed successfully." << std::endl;

    return 0;
}