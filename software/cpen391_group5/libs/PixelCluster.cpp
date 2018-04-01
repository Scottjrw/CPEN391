#include <PixelCluster.hpp>
#include "io.h"
#include <cassert>

PixelCluster::PixelCluster(uintptr_t base, int ic_id, int irq_id):
    m_base(reinterpret_cast<uint32_t *>(base)),
    m_ic_id(ic_id),
    m_irq_id(irq_id),
    m_last_data(),
    m_last_isr_ticks(0),
    m_accumlate_isr_ticks(0),
    m_accumulate_count(0),
    m_cb(nullptr),
    m_cb_isr(false),
    m_call_cb(false),
    m_compare0_val(0),
    m_compare1_val(0),
    m_range_val(0)
{
    reset();
    IOWR(m_base, Compare0_Register, m_compare0_val);
    IOWR(m_base, Compare1_Register, m_compare1_val);
    IOWR(m_base, Range_Register, m_range_val);
}

void PixelCluster::startIRQ() {

    reset();
    int status = alt_ic_isr_register(m_ic_id, m_irq_id, interrupt_handler, this, nullptr);

    assert(status == 0);

    status = alt_ic_irq_enable(m_ic_id, m_irq_id);

    assert(status == 0);

    reset();
}

void PixelCluster::stopIRQ() {
    int status = alt_ic_irq_disable(m_ic_id, m_irq_id);
    assert(status == 0);

}

bool PixelCluster::irq_overflow() {
    if (m_isr_overflow) {
        m_isr_overflow = false;
        return true;
    } else {
        return false;
    }
}

void PixelCluster::finish_cb(PixelClusterCB cb, bool use_isr) {
    m_cb = cb;
}

void PixelCluster::reset() {
    IOWR(m_base, Control_Register, 0);
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

void PixelCluster::range(uint16_t range) {
    IOWR(m_base, Range_Register, range);
}

void PixelCluster::compare_value(uint8_t r, uint8_t g, uint8_t b,
        uint32_t &reg, unsigned reg_num) {
    reg &= ~0xFFFFFF;
    reg |= (r << 16) | (g << 8) | (b);

    IOWR(m_base, reg_num, reg);
}

void PixelCluster::compare_enable(bool r, bool g, bool b,
        uint32_t &reg, unsigned reg_num) {
    reg &= ~0x7000000;

    uint8_t en_val = 0;
    en_val |= r ? 0b100 : 0;
    en_val |= g ? 0b010 : 0;
    en_val |= b ? 0b001 : 0;

    reg |= en_val << 24;

    IOWR(m_base, reg_num, reg);
}

void PixelCluster::compare_less_than(bool r, bool g, bool b,
        uint32_t &reg, unsigned reg_num) {
    reg &= ~0x38000000;

    uint8_t lt_val = 0;
    lt_val |= r ? 0b100 : 0;
    lt_val |= g ? 0b010 : 0;
    lt_val |= b ? 0b001 : 0;

    reg |= lt_val << 27;

    IOWR(m_base, reg_num, reg);
}

void PixelCluster::trypoll() {
    if (!m_cb_isr && m_call_cb) {
        if (m_cb != nullptr)
            m_cb(this, m_last_data);
        m_call_cb = false;
    }
}

void PixelCluster::interrupt_handler(void *pxcluster) {

    PixelCluster *pixel = static_cast<PixelCluster*>(pxcluster);

    // Frequency calculation
    unsigned ticks_diff = alt_nticks() - pixel->m_last_isr_ticks;
    pixel->m_accumlate_isr_ticks += ticks_diff;
    pixel->m_accumulate_count++;
    pixel->m_last_isr_ticks = alt_nticks();

    // Retrieve data
    for (unsigned i = 0; i < N_Clusters; i++) {
        ClusterData &cur = pixel->m_last_data[i];

        uint32_t avg_xy = IORD(pixel->m_base, Avg_XY_Register(i));
        cur.avg_x = avg_xy & 0xFFFF;
        cur.avg_y = avg_xy >> 16;

        uint32_t min_xy = IORD(pixel->m_base, Min_XY_Register(i));
        cur.min_x = min_xy & 0xFFFF;
        cur.min_y = min_xy >> 16;

        uint32_t max_xy = IORD(pixel->m_base, Max_XY_Register(i));
        cur.max_x = max_xy & 0xFFFF;
        cur.max_y = max_xy >> 16;

        cur.count = IORD(pixel->m_base, Count_Register(i));
    }

    if (!pixel->m_cb_isr && pixel->m_call_cb)
        pixel->m_isr_overflow = true;
    pixel->m_call_cb = true;

    if (pixel->m_cb_isr && pixel->m_cb != nullptr)
        pixel->m_cb(pixel, pixel->m_last_data);

    pixel->reset();

}
