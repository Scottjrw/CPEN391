#ifndef CLOSE_HANDLER_HPP
#define CLOSE_HANDLER_HPP
#include <csignal>
#include <err.h>
#include "event_loop.hpp"

namespace CloseHandler {
    namespace Detail {
        volatile bool should_exit = false;

        void signal_handler(int signal) {
            switch (signal) {
                case SIGTERM: 
                case SIGINT:
                    should_exit = true;
                    return;
                default:
                    errx(1, "CloseHandler: got signal %d\n", signal);
            };
        }
    }



    /*
     * Build a new trypoll function that can be used in a event loop
     * the ret_val specified will be used to stop the event loop
     *
     */
    Event_Loop::Callable make_exit_trypoll(int ret_val) {
        using Detail::should_exit;

        std::signal(SIGINT, Detail::signal_handler);
        std::signal(SIGTERM, Detail::signal_handler);

        return [ret_val] (Event_Loop *ev) {
            if (Detail::should_exit) {
                ev->stop(ret_val);
            }
        };

    }

};

#endif // CLOSE_HANDLER_HPP
