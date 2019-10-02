#include <iostream>
#include <chrono>
#include <thread>
#include "shm.h"

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

int main(int argc, char *argv[]) {
    // No args == "Server"
    if (argc == 1) {
        auto* shmObj = shm_create<MyObj>();

        if (shmObj == nullptr) {
            std::cerr << "Failed to create shared object" << std::endl;
            return 1;
        }
        std::cout << "Created shm object" << std::endl;

        while (shmObj->running()) {
            std::cout << "Shm Val: " << shmObj->val() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        shm_destroy(shmObj);
        std::cout << "Good bye!";

    } else {
        auto* shmObj = shm_connect<MyObj>();

        if (shmObj == nullptr) {
            std::cerr << "Failed to connect to shared memory" << std::endl;
            return 1;
        }

        int x = atoi(argv[1]);
        if (x >= 0) {
            shmObj->val(x);
        } else {
            // Stop on negative argv[1]
            shmObj->stop();
        }

        shm_free(shmObj);
    }
    return 0;
}