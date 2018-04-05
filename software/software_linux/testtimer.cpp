#include <iostream>
#include "event_loop.hpp"
#include <unistd.h>
#include <time.h>

using namespace std::literals::chrono_literals;

uint64_t get_us_time() {
    timespec t;

    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec*1000000 + t.tv_nsec / 1000;
}

int main(void) {
    Event_Loop ev;

    uint64_t last_1 = get_us_time();

    ev.add_timer(1s, [&last_1] (auto)->bool {
        std::cout << "Every 1 seconds, " << get_us_time() - last_1 << std::endl;
        last_1 = get_us_time();
        return true;
    });

    uint64_t last_05 = get_us_time();

    ev.add_timer(500ms, [&last_05] (auto)->bool {
        std::cout << "Every 0.5 second, " << get_us_time() - last_05 << std::endl;
        last_05 = get_us_time();
        return true;
    });

    unsigned count = 0;

    ev.add_timer(100ms, [&count] (auto)->bool {
        std::cout << "10ms" << std::endl;
        if (++count == 100) {
            std::cout << "Every 10ms" << std::endl;
            count = 0;
        }
        return true;
    });


    ev.add_timer(1s, [](Event_Loop *evp)->bool {
        std::cout << "Start" << std::endl;
        auto start = std::chrono::system_clock::now();
        evp->sleep(500ms);
        std::chrono::microseconds t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);

        std::cout << "Microseconds " << t.count() << std::endl;
        return true;
    });

    unsigned count2 = 0;
    Event_Loop::CallableRef ref;

    ref = ev.add([&ref, &count2](Event_Loop *evp) {
        if (count2++ <= 5) {
            std::cout << "Wait..." << std::endl;
            auto start = std::chrono::system_clock::now();
            evp->sleep(750ms);
            std::chrono::microseconds t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);

            std::cout << "Wait Microseconds " << t.count() << std::endl;

        } else {
            std::cout << "Removing!" << std::endl;
            evp->remove(ref);
        }
    });

    ev.run();
}
