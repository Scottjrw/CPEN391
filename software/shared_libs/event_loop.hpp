#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include <cassert>
#include <functional>
#include <vector>
#include <chrono>
#include <queue>
#include <iostream>

class Event_Loop {
public:
    Event_Loop() :
        m_run(false), m_callables(), m_timers() {}

    typedef std::function<void(Event_Loop *)> Callable;

    /* Add a regular function to call, function should not block when it has nothing to do
     */
    void add(Callable trypoll, int pos=-1) {
        assert(trypoll != nullptr);
        if (pos < 0)
            m_callables.push_back(trypoll);
        else
            m_callables.insert(m_callables.begin() + pos, trypoll);
    }

    /* Add a member function, pass in the "this" of the object and its member function
     */
    template <typename T>
    void add(T *c, void (T::*trypoll)(Event_Loop *)) {
        assert(trypoll != nullptr);
        add(std::bind(trypoll, c, std::placeholders::_1));
    }

    typedef std::function<bool(Event_Loop *)> TimerCallable;
    typedef std::chrono::microseconds TimerDuration;

    /* Add a new timer with period. The timer's first call will be at now() + period
     * A timer returns true to continue receiving callbacks otherwise the timer is stopped.
     *
     * Timers generally don't achieve microsecond accuracy due to other callbacks being
     * run in the event loop, but we can guarentee a timer will not drift off its period
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

    /* Run the loop, which repeatedly calls the added trypoll functions
     * 
     * The loop returns when stop is called, and returns the value passed to stop
     */
    int run() {
        m_run = true;
        while (1) {
            for (Callable &c : m_callables) {
                if (m_run) {
                    c(this);
                    checktimers();
                } else return m_ret;
            }

            if (m_run) {
                checktimers();
            } else return m_ret;
        }
    }

    void stop(int ret) {
        assert(m_run = true);
        m_run = false;
        m_ret = ret;
    }


private:
    bool m_run;
    int m_ret;

    inline void checktimers() {
        if (m_timers.top().end <= std::chrono::high_resolution_clock::now()) {
            const Timer &t = m_timers.top();
            if (t.cb(this))
                m_timers.emplace(t.cb, t.period, t.end);
            m_timers.pop();
        }
    }

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

    std::vector<Callable> m_callables;
    std::priority_queue<Timer> m_timers;
};

#endif // EVENT_LOOP_HPP
