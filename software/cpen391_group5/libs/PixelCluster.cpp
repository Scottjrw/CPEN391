#include <PixelCluster.hpp>
#include "io.h"
#include <cassert>

void PixelCluster::startIRQ() {

    reset();
    int status = alt_ic_isr_register(m_ic_id, m_irq_id, gesture_interrupt, this, nullptr);

    assert(status == 0);

    status = alt_ic_irq_enable(m_ic_id, m_irq_id);

    assert(status == 0);

    reset();
}

void PixelCluster::stopIRQ() {
    int status = alt_ic_irq_disable(m_ic_id, m_irq_id);
    assert(status == 0);

}

void PixelCluster::isrCB(PixelCluster::ClusterCB cb) {
    m_cb = cb;
}

void PixelCluster::reset() {
    IOWR_32DIRECT(m_base, Control_Reg, 0);
}

void PixelCluster::poll(unsigned &x, unsigned &y, unsigned &count) {
    x = m_last_x;
    y = m_last_y;
    count = m_last_count;
}

unsigned PixelCluster::isr_period_ms(bool reset) {
    unsigned ms = 0;

    if (m_accumulate_count != 0)
        ms = 1000 * m_accumlate_isr_ticks / alt_ticks_per_second() / m_accumulate_count;

    if (reset) {
        m_accumulate_count = 0;
        m_accumlate_isr_ticks = 0;
    }

    return ms;
}

void PixelCluster::compare_value(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t pix = (r << 16) | (g << 8) | (b);

    IOWR_32DIRECT(m_base, Compare_Color_Register, pix);
}

void PixelCluster::compare_enable(bool r, bool g, bool b) {
    uint32_t prev = IORD_32DIRECT(m_base, Compare_En_Reg);

    // Preserve the less than bits
    prev &= 0xFF00;

    uint8_t en_val = 0;
    en_val |= r ? 0b100 : 0;
    en_val |= g ? 0b010 : 0;
    en_val |= b ? 0b001 : 0;

    IOWR_32DIRECT(m_base, Compare_En_Reg, prev | en_val);
}

void PixelCluster::compare_less_than(bool r, bool g, bool b) {
    uint32_t prev = IORD_32DIRECT(m_base, Compare_En_Reg);

    // Preserve the enable bits
    prev &= 0xFF;

    uint8_t lt_val = 0;
    lt_val |= r ? 0b100 : 0;
    lt_val |= g ? 0b010 : 0;
    lt_val |= b ? 0b001 : 0;

    IOWR_32DIRECT(m_base, Compare_En_Reg, prev | (lt_val << 8));
}

void PixelCluster::range(uint16_t range) {
    IOWR_32DIRECT(m_base, Range_Register, range);
}

void PixelCluster::gesture_interrupt(void *gestureptr) {

    PixelCluster *gesture = static_cast<PixelCluster*>(gestureptr);

    // Frequency calculation
    unsigned ticks_diff = alt_nticks() - gesture->m_last_isr_ticks;
    gesture->m_accumlate_isr_ticks += ticks_diff;
    gesture->m_accumulate_count++;
    gesture->m_last_isr_ticks = alt_nticks();

    gesture->m_last_x = IORD_32DIRECT(gesture->m_base, Max_X_Register);
    gesture->m_last_y = IORD_32DIRECT(gesture->m_base, Max_Y_Register);
    gesture->m_last_count = IORD_32DIRECT(gesture->m_base, Max_Count_Register);

    ClusterCB cb = gesture->m_cb;

    if (cb != nullptr)
        cb(gesture, gesture->m_last_x, gesture->m_last_y, gesture->m_last_count);

    gesture->reset();

}
