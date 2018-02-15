#include <stdint.h>
#include <functional>
#include <iostream>
#include <sys/alt_irq.h>
#include <sys/alt_alarm.h>

class PixelCluster {
public:

    typedef std::function<void(PixelCluster *, unsigned x, unsigned y, unsigned count)> ClusterCB;

    void startIRQ();

    // Set callback
    void isrCB(ClusterCB cb);

    void reset();

    void poll(unsigned &x, unsigned &y, unsigned &count);

    // Get average period at which the isr is triggered
    // reset will reset the period stats
    unsigned isr_period_ms(bool reset=true);

    // Settings
    // color values to compare against
    void compare_value(uint8_t r, uint8_t g, uint8_t b);

    // if a color should be compared
    void compare_enable(bool r, bool g, bool b);

    // if colors should be less than the compare value
    void compare_less_than(bool r, bool g, bool b);

    void range(uint16_t range);

    PixelCluster(uint32_t *base, int ic_id, int irq_id):
        m_base(base),
        m_ic_id(ic_id),
        m_irq_id(irq_id),
        m_last_x(0),
        m_last_y(0),
        m_last_count(0),
        m_last_isr_ticks(0),
        m_accumlate_isr_ticks(0),
        m_accumulate_count(0),
        m_cb(nullptr)
    {}

private:
    uint32_t * const m_base;
    const alt_u32 m_ic_id;
    const alt_u32 m_irq_id;
    volatile unsigned m_last_x, m_last_y, m_last_count;
    unsigned m_last_isr_ticks;
    unsigned m_accumlate_isr_ticks;
    unsigned m_accumulate_count;
    ClusterCB m_cb;

    static void gesture_interrupt(void *);

    static constexpr uintptr_t Control_Reg =              0;
    static constexpr uintptr_t Compare_En_Reg =           4;
    static constexpr uintptr_t Compare_Color_Register =   8;
    static constexpr uintptr_t Range_Register =           12;
    static constexpr uintptr_t Max_X_Register =           16;
    static constexpr uintptr_t Max_Y_Register =           20;
    static constexpr uintptr_t Max_Count_Register =       24;
    static constexpr uintptr_t Debug_0_Register =         28;
    static constexpr uintptr_t Debug_1_Register =         32;
    static constexpr uintptr_t Debug_2_Register =         36;
    static constexpr uintptr_t Debug_3_Register =         40;

};
