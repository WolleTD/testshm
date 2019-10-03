#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>
#include <atomic>
#include "shm.h"

std::atomic_bool quit(false);

class MyObj {
    uint32_t _val {0};
    bool _running {true};
public:
    MyObj() = default;
    uint32_t val() { return _val; }
    void val(uint32_t i) { _val = i; }
    bool running() { return _running; }
    void stop() { _running = false; }
};

static void sighandler(int) {
    quit = true;
}

int main(int argc, char *argv[]) {
    // No args == "Server"
    if (argc == 1) {
        struct sigaction sa;
        sa.sa_handler = sighandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sigaction(SIGINT, &sa, nullptr);

        auto shmObj = ShmObject<MyObj>(typeid(MyObj).name());

        std::cout << "Created shm object" << std::endl;
        auto lastTime = std::chrono::steady_clock::now();

        while (shmObj().running() && !quit) {
            auto thisTime = std::chrono::steady_clock::now();
            std::cout << "Shm Val: " << shmObj().val() << " time " <<
                std::chrono::duration_cast<std::chrono::microseconds>(thisTime - lastTime).count() <<
                "µs" << std::endl;
            lastTime = thisTime;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "Good bye!";

    } else {
        auto shmObj = ShmObject<MyObj>(typeid(MyObj).name(), ShmObject<MyObj>::attach_rw);

        int x = atoi(argv[1]);
        if (x >= 0) {
            shmObj().val(x);
        } else {
            // Stop on negative argv[1]
            shmObj().stop();
        }
    }
    return 0;
}