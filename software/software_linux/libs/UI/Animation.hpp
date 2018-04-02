#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <functional>
#include <chrono>
#include "UI.hpp"
#include "event_loop.hpp"

namespace UI {

template <class T=int>
class Animation {
public:
    typedef std::function<void(T val, bool end)> AnimationCB;

    Animation(T begin_val, T end_val, T increment, AnimationCB cb=nullptr):
        m_cb(cb), m_begin_val(begin_val), m_end_val(end_val), 
        m_increment(increment), m_current_val(begin_val)
    {
        assert(m_begin_val != m_end_val);
    }

    void run(Event_Loop &ev) {
        assert(m_cb != nullptr);
        m_current_val = m_begin_val;
        ev.add_timer(TIMER_PERIOD, this, &Animation::animate_cb);
    }

    void cb(AnimationCB cb) {
        assert(m_cb == nullptr && cb != nullptr);
        m_cb = cb;
    }

private:
    AnimationCB m_cb;
    T m_begin_val, m_end_val, m_increment;
    T m_current_val;

    static constexpr unsigned FRAME_RATE = 24;
    static constexpr Event_Loop::TimerDuration TIMER_PERIOD = 
        std::chrono::milliseconds(1000/FRAME_RATE);

    bool animate_cb(Event_Loop *ev) {
        bool end = false;
        T new_val = m_current_val + m_increment;

        end = (m_begin_val > m_end_val && new_val < m_end_val)
           || (m_begin_val < m_end_val && new_val > m_end_val);

        m_cb(m_current_val, end);
        m_current_val = new_val;

        return !end;
    }
};

};

#endif //ANIMATION_HPP
