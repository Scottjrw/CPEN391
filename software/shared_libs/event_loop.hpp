#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include <cassert>
#include <functional>
#include <vector>

class Event_Loop {
public:
    Event_Loop() :
        m_run(false), m_callables() {}

    typedef std::function <void(Event_Loop *)> Callable;

    // Add a regular function to call, function should not block when it has nothing to do
    void add(Callable trypoll, int pos=-1) {
        if (pos < 0)
            m_callables.push_back(trypoll);
        else
            m_callables.insert(m_callables.begin() + pos, trypoll);
    }

    // Add a member function, pass in the "this" of the object and its member function
    template <typename T>
    void add(T *c, void (T::*trypoll)(void)) {
        assert(trypoll != nullptr);
        add(std::bind(trypoll, c));
    }

    template <typename T>
    void add(T *c, void (T::*trypoll)(Event_Loop *)) {
        assert(trypoll != nullptr);
        add(std::bind(trypoll, c, std::placeholders::_1));
    }

    // Run the loop, which repeatedly calls the added trypoll functions
    int run() {
        m_run = true;
        while (1) {
            for (Callable &c : m_callables) {
                if (m_run)
                    c(this);
                else
                    return m_ret;
            }
        }
    }

    // Called by one of the callbacks to stop the event loop
    void stop(int ret) {
        assert(m_run = true);
        m_run = false;
        m_ret = ret;
    }


private:
    bool m_run;
    int m_ret;
    std::vector<Callable> m_callables;

};

#endif // EVENT_LOOP_HPP
