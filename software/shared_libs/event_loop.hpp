#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include <cassert>
#include <functional>
#include <vector>
#include <chrono>
#include <queue>
#include <iostream>
#include <list>

#ifdef __hal__
#define EV_NIOS
#else
#define EV_ARM
#endif

/*
 * Event Loop is a way to implement many asynchronous tasks without using multithreading.
 *
 * Generally we can register many functions called "trypolls" to be called continuously.
 * These trypolls will check if they required to perform an action if not they return
 * immediately.
 * An examples would be checking for UART messages: the trypoll just checks if there are
 * characters in the UART, if there are it will process them otherwise it just returns.
 * Thus by following this pattern we can poll multiple UART ports at the same time in the same
 * thread.
 *
 * We also provide a timer interface to efficiently make period or just single shot timers.
 * These are disabled on the NIOS version as they are unnecessary and introduce more overhead.
 *
 */
class Event_Loop {
private:
    struct _Callable {
        std::function<void(Event_Loop *)> callable;
        bool running;
        bool dead;

        _Callable(std::function<void(Event_Loop *)> callable):
            callable(callable), running(true), dead(false) {}
    };
public:

    Event_Loop() :
        m_run(false),  m_ret(0), m_callables() {}


    typedef std::function<void(Event_Loop *)> Callable;
    typedef std::list<_Callable>::iterator CallableRef;
    /* Add a regular function to call, function should not block when it has nothing to do
     *
     * add() returns a reference to the callback. This refernce can be used in remove() to
     * stop the trypoll. If one does not need to call remove the return value can be ignored.
     */
    CallableRef add(Callable trypoll) {
        assert(trypoll != nullptr);
        m_callables.emplace_back(trypoll);
        auto end = m_callables.end();
        end--;

        return end;
    }

    /* Add a member function, pass in the "this" of the object and its member function
     */
    template <typename T>
    CallableRef add(T *c, void (T::*trypoll)(Event_Loop *)) {
        assert(trypoll != nullptr);
        return add(std::bind(trypoll, c, std::placeholders::_1));
    }

    template <typename T>
    CallableRef add(T *c, void (T::*trypoll)(void)) {
        assert(trypoll != nullptr);
        return add(std::bind(trypoll, c));
    }

    /* Remove a callback, needs a reference to the callback
     *
     */
    void remove(CallableRef ref) {
        ref->dead = true;
        ref->running = false;
    }

#ifdef EV_ARM
    typedef std::function<bool(Event_Loop *)> TimerCallable;
    typedef std::chrono::microseconds TimerDuration;

    /* Add a new timer with period. The timer's first call will be at now() + period
     * A timer returns true to continue receiving callbacks otherwise the timer is stopped.
     *
     * Timers are guarenteed to have no drift. But their accuracy isn't guarenteed. It is
     * possible that some other trypoll/timer blocks a timer's execution so that it misses
     * its expected time.
     */
    template <class Rep, class Period> 
    void add_timer(std::chrono::duration<Rep, Period> period, TimerCallable timer_cb) {
        assert(timer_cb != nullptr);

        m_timers.emplace(timer_cb, std::chrono::duration_cast<TimerDuration>(period));
    }

    template <typename T, class Rep, class Period> 
    void add_timer(std::chrono::duration<Rep, Period> period, T *c, bool (T::*timer_cb)(Event_Loop *)) {
        add_timer(period, std::bind(timer_cb, c, std::placeholders::_1));
    }

    template <typename T, class Rep, class Period> 
    void add_timer(std::chrono::duration<Rep, Period> period, T *c, bool (T::*timer_cb)(void)) {
        add_timer(period, std::bind(timer_cb, c));
    }
#endif // EV_ARM

    /* Run the loop, which repeatedly calls the added trypoll functions
     * 
     * The loop returns when stop is called, and returns the value passed to stop
     */
    int run() {
        assert(!m_run);
        m_run = true;

        unsigned count = 0;

        while (m_run) {
            for (auto cur = m_callables.begin(); cur != m_callables.end(); cur++) {
                if (m_run && cur->running)
                    cur->callable(this);
                if (cur->dead)
                    cur = m_callables.erase(cur);

                checktimers();

            }

            checktimers();

            if (count++ >= 256) {
                m_callables.remove_if([] (const _Callable &i) -> bool {
                    return i.dead;
                });

                count = 0;
            }
        }

        return m_ret;
    }

    // Sleep and cont are removed due to issues
    //inline bool cont() {
    //    assert(m_run);
    //    auto cur = m_cur;
    //    bool cur_is_timer = m_is_timer;

    //    if (!cur_is_timer) {
    //        cur->running = false;
    //    }
    //    m_is_timer = false;

    //    runOnce();

    //    if (!cur_is_timer && !cur->dead)
    //        cur->running = true;

    //    m_cur = cur;

    //    return m_run;
    //}

#ifdef EV_ARM
    //template <class Rep, class Period> 
    //bool sleep(std::chrono::duration<Rep, Period> period) {
    //    assert(m_run);
    //    auto end_time = std::chrono::high_resolution_clock::now() + period;

    //    while (end_time > std::chrono::high_resolution_clock::now() && cont());

    //    return m_run;
    //}
#endif // EV_ARM

    void stop(int ret) {
        assert(m_run);
        m_run = false;
        m_ret = ret;
    }


private:
    bool m_run;
    int m_ret;

    inline void checktimers() {
#ifdef EV_ARM
        if (m_run && !m_timers.empty()) {
            if (m_timers.top().end <= std::chrono::high_resolution_clock::now()) {
                const Timer &t = m_timers.top();
                if (t.cb(this))
                    m_timers.emplace(t.cb, t.period, t.end);
                m_timers.pop();
            }
        }
#endif // EV_ARM
    }

#ifdef EV_ARM
    typedef std::chrono::time_point<std::chrono::high_resolution_clock, TimerDuration> TimerPoint;

    struct Timer {
        TimerCallable cb;
        TimerDuration period;
        TimerPoint end;

        // Reversed since we want the priority queue to sort by increasing times
        bool operator< (const Timer &other) const {
            return end > other.end;
        }

        // Build a new timer with period
        Timer(const TimerCallable &cb, TimerDuration period):
            cb(cb), 
            period(period), 
            end(std::chrono::time_point_cast<TimerDuration>(std::chrono::high_resolution_clock::now()) + period)
        {
            std::cout << "Timer with period: " << period.count() << std::endl;
        }

        // Renew timer, reuse the old end time to prevent timer drift
        Timer(const TimerCallable &cb, TimerDuration period, TimerPoint end):
            cb(cb),
            period(period),
            end(end + period)
        {}
    };

    /* Use a priority heap to implement timers. By doing so we can just check the top
     * of the heap to determine if any timers need to be triggered. Heaps also provide
     * better performance than sorted arrays/lists when only the top matters.
     *
     */
    std::priority_queue<Timer> m_timers;
#endif // EV_ARM

    std::list<_Callable> m_callables;

};

#endif // EVENT_LOOP_HPP
