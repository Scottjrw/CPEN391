#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include <cassert>
#include <functional>
#include <vector>
#include <chrono>
#include <queue>
#include <iostream>

#ifdef __hal__
#define EV_NIOS
#else
#define EV_ARM
#endif

class Event_Loop {
public:
    Event_Loop() :
        m_run(false), m_is_timer(false), m_is_valid(false), m_ret(0), m_callables(), m_cur() {}

    typedef std::function<void(Event_Loop *)> Callable;
    typedef std::vector<Callable>::const_iterator CallableRef;

    /* Add a regular function to call, function should not block when it has nothing to do
     */
    CallableRef add(Callable trypoll) {
        assert(trypoll != nullptr);
        m_callables.push_back(trypoll);
        invalidate();
        return m_callables.cend() - 1;
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

    void remove(CallableRef ref) {
        m_callables.erase(ref);
        invalidate();
    }

#ifdef EV_ARM
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
        while (1) {
            for (m_cur = m_callables.begin(); m_cur != m_end; m_cur++) {
                if (m_run)
                    (*m_cur)(this);
                else return m_ret;

                if (m_run)
                    checktimers();
                else return m_ret;
            }

            if (m_run) {
                checktimers();
            } else return m_ret;
        }
    }

    /* Inside a trypoll this can be called to run the loop once. This is useful
     * if implementing a polling wait without blocking loop execution.
     *
     * The event loop may be stopped during a call to cont. The return value of this
     * call is whether or not the loop is still running. Most of the time this can
     * be ignored.
     *
     * Trying to call this within a timer callback is a violation as is trying to
     * call this when the event loop is not running.
     */
    bool cont() {
        assert(m_run && !m_is_timer);
        auto last_cur = m_cur;
        m_is_valid = true;

        for (m_cur = m_callables.begin(); m_cur != m_end; m_cur++) {
            if (m_cur != last_cur) {
                if (m_run)
                    (*m_cur)(this);
                else return false;
            }

            if (m_run)
                checktimers();
            else return false;
        }

        if (m_run) {
            checktimers();
        } else return false;

        if (m_is_valid) {
            m_cur = last_cur;
        }

        return m_run;
    }

#ifdef EV_ARM
    /* A sleep which runs the event loop for period time
     * This allows a function to block itself for awhile without
     * blocking event loop execution.
     * 
     * Like cont(), this cannot be called in a timer or when the
     * event loop is not running
     */
    template <class Rep, class Period> 
    bool sleep(std::chrono::duration<Rep, Period> period) {
        assert(m_run && !m_is_timer);
        auto end_time = std::chrono::high_resolution_clock::now() + period;

        while (end_time > std::chrono::high_resolution_clock::now() && cont());
    }
#endif // EV_ARM

    void stop(int ret) {
        assert(m_run);
        m_run = false;
        m_ret = ret;
    }


private:
    bool m_run;
    bool m_is_timer;
    bool m_is_valid;
    int m_ret;

    inline void checktimers() {
#ifdef EV_ARM
        if (!m_timers.empty()) {
            if (m_timers.top().end <= std::chrono::high_resolution_clock::now()) {
                const Timer &t = m_timers.top();
                m_is_timer = true;
                if (t.cb(this))
                    m_timers.emplace(t.cb, t.period, t.end);
                m_is_timer = false;
                m_timers.pop();
            }
        }
#endif // EV_ARM
    }

    // Invalidate any running loop when modifying the callables
    inline void invalidate() {
        m_end = m_callables.end();
        m_cur = m_end;
        m_is_valid = false;
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
    std::priority_queue<Timer> m_timers;
#endif // EV_ARM

    std::vector<Callable> m_callables;
    std::vector<Callable>::iterator m_cur;
    std::vector<Callable>::iterator m_end;
};

#endif // EVENT_LOOP_HPP
