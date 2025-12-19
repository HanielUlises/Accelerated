#include <iostream>
#include <condition_variable>

class Semaphore{
        std::mutex mut;
        std::condition_variable cv;
        int counter{0};
        int max_counter = 10;
    public:
        void release() {
            std::lock_guard<std::mutex> lock(mut);
            std::cout << "Adding one item " << '\n';
            
            if(counter < max_counter) {
                ++counter;
                count();
            }        

            cv.notify_all();
        }

        void acquire() {
            std::unique_lock<std::mutex> lock(mut);
            std::cout << "Removing one item " << '\n';

            while(counter == 0) {
                cv.wait(lock);
            }

            --counter;
            count();
        }

        void count() const{
            std::cout << "Value of counter: " << counter << '\n';
        }
};