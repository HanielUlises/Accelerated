#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <string>

using namespace std::literals;

std::string s_data;

/* Flag for thread communication */
bool update_progress = false;
bool completed = false;

std::mutex data_mutex;
std::mutex completed_mutex;

void fetch_data() {
    for(int i = 0; i < 5; ++i) {
        std::cout << "Fetcher thread waiting for data..." << std::endl;
        std::this_thread::sleep_for(2s);

        // Update data -> Notify progress bar
        std::lock_guard<std::mutex> data_lock(data_mutex);
        s_data += "Block " + std::to_string(i + 1);
        std::cout << "s_data: " << s_data << " " <<std::endl;
        update_progress = true;
    }

    std::cout << "Fetching data completed." << std::endl;

    // Exit the progress data and wake up the processing thread
    std::lock_guard<std::mutex> completed_lock(completed_mutex);
}

void progress_bar() {
    size_t length = 0;

    while(true) {
        std::cout << "Progress bar thread waiting for data..." << std::endl;

        // Wait until there's new data to display
        std::unique_lock<std::mutex> data_lock(data_mutex);
        
        while (!update_progress) {
            data_lock.unlock();
            std::this_thread::sleep_for(10ms);
            data_lock.lock();
        }
        
        length = s_data.size();

        update_progress = false;
        data_lock.unlock();

        std::cout << "Received " << length << " bytes so far." << std::endl;

        // Terminate when fetching whole data batch
        std::lock_guard<std::mutex> completed_lock(completed_mutex);
        if(completed) {
            std::cout << "Progress bar thread has ended " << std::endl;
            break;
        }
    }
}

void process_data() {
    std::cout << "Processing thread waiting for data..." << std::endl;
    std::unique_lock<std::mutex> completed_lock(data_mutex);

    while(!completed) {
        completed_lock.unlock();
        std::this_thread::sleep_for(10ms);
        completed_lock.lock();
    }

    completed_lock.unlock();
    std::lock_guard<std::mutex> data_lock(data_mutex);
    std::cout << "Processing s_data: " << s_data << std::endl;
}

int main() {
    std::thread fetcher(fetch_data);
    std::thread progress(progress_bar);
    std::thread process(process_data);

    fetcher.join(); progress.join(); process.join();

    return 0;
}
