#include <iostream>
#include <chrono>
#include <thread>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

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
    using namespace boost::interprocess;
    // No args == "Server"
    if (argc == 1) {
        shared_memory_object shm(create_only, "mySHM", read_write);
        shm.truncate(sizeof(MyObj));
        mapped_region region(shm, read_write);
        auto* shmObj = new (region.get_address()) MyObj();

        while (shmObj->running()) {
            std::cout << "Shm Val: " << shmObj->val() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << "Good bye!";
        shared_memory_object::remove("mySHM");
    } else {
        shared_memory_object shm(open_only, "mySHM", read_write);
        mapped_region region(shm, read_write);
        auto* shmObj = (MyObj*)region.get_address();

        int x = atoi(argv[1]);
        if (x >= 0) {
            shmObj->val(x);
        } else {
            // Stop on negative argv[1]
            shmObj->stop();
        }
    }
    return 0;
}