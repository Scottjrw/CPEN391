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

    ev.add_timer(10ms, [&count] (auto)->bool {
        if (++count == 100) {
            std::cout << "Every 10ms" << std::endl;
            count = 0;
        }
        return true;
    });

    ev.run();

#if 0
    std::cout << "Test: " << (1s + 1000ms).count() << std::endl;

    auto t = std::chrono::high_resolution_clock::now();

    t += 1s;

    while(std::chrono::high_resolution_clock::now() < t) {
        std::cout << std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000 << std::endl;
        usleep(100000);
    }

#endif
}
