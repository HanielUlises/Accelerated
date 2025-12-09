#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

struct Config {
    int level;
    std::string mode;
};

// Readers always see a consistent snapshot, writers swap atomically.
class ConfigManager {
public:
    void update(Config c) {
        auto new_cfg = std::make_shared<Config>(std::move(c));
        cfg.store(new_cfg, std::memory_order_release);
    }

    std::shared_ptr<const Config> current() const {
        return cfg.load(std::memory_order_acquire);
    }

private:
    std::atomic<std::shared_ptr<const Config>> cfg {
        std::make_shared<Config>(Config{0, "init"})
    };
};

// Readers continuously fetch config, writer updates it periodically.
int main() {
    ConfigManager manager;

    std::thread reader([&] {
        for (int i = 0; i < 5; ++i) {
            auto c = manager.current();
            std::cout << "Reader sees: level=" << c->level
                      << ", mode=" << c->mode << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    });

    std::thread writer([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        manager.update({1, "alpha"});
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        manager.update({2, "beta"});
    });

    reader.join();
    writer.join();
}
