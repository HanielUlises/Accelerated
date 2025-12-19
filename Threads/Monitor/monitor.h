#include <mutex>
#include <condition_variable>
#include <utility>


#ifdef PREDICATIVE
// Base class providing monitor functionality
class Monitor {
protected:
    // Protected constructor - only derived classes can instantiate
    Monitor() = default;
    ~Monitor() = default;  // Non-virtual: not meant for polymorphic deletion

    // Mutual exclusion
    mutable std::mutex mtx_;

    // Condition variable for waiting and signaling
    mutable std::condition_variable cond_;

public:
    // Delete copy and move to prevent accidental sharing
    Monitor(const Monitor&) = delete;
    Monitor& operator=(const Monitor&) = delete;
    Monitor(Monitor&&) = delete;
    Monitor& operator=(Monitor&&) = delete;

    // Lock and unlock the monitor
    void lock() const {
        mtx_.lock();
    }

    void unlock() const {
        mtx_.unlock();
    }

    // Notify one or all waiting threads
    void notify_one() const noexcept {
        cond_.notify_one();
    }

    void notify_all() const noexcept {
        cond_.notify_all();
    }

    // Wait until a predicate becomes true
    template <typename Predicate>
    void wait(Predicate&& stop_waiting) const {
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, std::forward<Predicate>(stop_waiting));
    }

    template <typename Predicate, typename Duration>
    bool wait_for(Predicate&& stop_waiting, const Duration& timeout) const {
        std::unique_lock<std::mutex> lock(mtx_);
        return cond_.wait_for(lock, timeout, std::forward<Predicate>(stop_waiting));
    }

    template <typename Predicate, typename Rep, typename Period>
    bool wait_for(Predicate&& stop_waiting,
                  const std::chrono::duration<Rep, Period>& timeout) const {
        std::unique_lock<std::mutex> lock(mtx_);
        return cond_.wait_for(lock, timeout, std::forward<Predicate>(stop_waiting));
    }
};

#endif

#pragma region Base

template <typename T>
class Monitor{
        // The object to be monitored
        T data;

        // Mutex to protect the data
        std::mutex data_mut;

    public:

        Monitor<T> (T data = {}) : data(data) {}

        template<typename Func>
        auto operator() (Func func) {
            std::lock_guard<std::mutex> lck_grd(data_mut);
            
            return func(data);
        }
};
