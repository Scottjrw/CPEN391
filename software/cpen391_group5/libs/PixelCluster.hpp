#ifndef PIXELCLUSTER_HPP
#define PIXELCLUSTER_HPP
#include <stdint.h>
#include <functional>
#include <iostream>
#include <array>
#include <sys/alt_irq.h>
#include <sys/alt_alarm.h>

class PixelCluster {
public:
    static constexpr unsigned N_Clusters = 4;

    struct ClusterData {
        unsigned avg_x, avg_y;
        unsigned min_x, min_y; 
        unsigned max_x, max_y;
        unsigned count;
    };

    typedef std::array<ClusterData, N_Clusters> NClusterData;

    typedef std::function<void(PixelCluster *, const NClusterData&)> PixelClusterCB;

    void startIRQ();
    void stopIRQ();
    bool irq_overflow();

    void reset();

    // Set callback, if use_isr is false, trypoll needs to be called to get a callback
    void finish_cb(PixelClusterCB cb, bool use_isr=false);

    // Get average period at which the isr is triggered
    // reset will reset the period stats
    unsigned isr_period_ms(bool reset=true);

    // Used for non-isr callbacks
    void trypoll();

    /*  ----  Settings  ----  */
    // Range of clustering
    void range(uint16_t range);

    // There are two comparators 0, 1. They are in AND mode, so both must match
    // One or both can be disabled using their enable method
    // 
    // value(r, g, b) - the color channels to compare against
    // enable(r, g, b) - whether or not that color channel should be compared
    //                   setting this to all false completely disables it
    // less_than(r, g, b) - false = pixel >= value
    //                      true  = pixel <  value

    void compare0_value(uint8_t r, uint8_t g, uint8_t b) {
        compare_value(r, g, b, m_compare0_val, Compare0_Register);
    }

    void compare0_enable(bool r, bool g, bool b) {
        compare_enable(r, g, b, m_compare0_val, Compare0_Register);
    }

    void compare0_less_than(bool r, bool g, bool b) {
        compare_less_than(r, g, b, m_compare0_val, Compare0_Register);
    }

    void compare1_value(uint8_t r, uint8_t g, uint8_t b) {
        compare_value(r, g, b, m_compare1_val, Compare1_Register);
    }

    void compare1_enable(bool r, bool g, bool b) {
        compare_enable(r, g, b, m_compare1_val, Compare1_Register);
    }

    void compare1_less_than(bool r, bool g, bool b) {
        compare_less_than(r, g, b, m_compare1_val, Compare1_Register);
    }

    PixelCluster(uintptr_t base, int ic_id, int irq_id);

private:
    uint32_t * const m_base;
    const alt_u32 m_ic_id;
    const alt_u32 m_irq_id;

    NClusterData m_last_data;

    unsigned m_last_isr_ticks;
    unsigned m_accumlate_isr_ticks;
    unsigned m_accumulate_count;

    PixelClusterCB m_cb;
    bool m_cb_isr;
    bool m_call_cb;
    bool m_isr_overflow;

    uint32_t m_compare0_val;
    uint32_t m_compare1_val;
    uint32_t m_range_val;

    static void interrupt_handler(void *);

    void compare_value(uint8_t r, uint8_t g, uint8_t b,
            uint32_t &reg, unsigned reg_num);

    void compare_enable(bool r, bool g, bool b,
            uint32_t &reg, unsigned reg_num);

    void compare_less_than(bool r, bool g, bool b,
            uint32_t &reg, unsigned reg_num);

    // Settings Registers
    static constexpr unsigned Control_Register =    0;
    static constexpr unsigned Compare0_Register =   1;
    static constexpr unsigned Compare1_Register =   2;
    static constexpr unsigned Range_Register =      3;

    // Result Registers
    static constexpr unsigned Registers_Per_Cluster = 4;
    static constexpr unsigned Clusters_Start = 4;

    static constexpr unsigned Avg_XY_Register(unsigned cluster) {
        return Registers_Per_Cluster*cluster + Clusters_Start + 0;
    }

    static constexpr unsigned Min_XY_Register(unsigned cluster) {
        return Registers_Per_Cluster*cluster + Clusters_Start + 1;
    }

    static constexpr unsigned Max_XY_Register(unsigned cluster) {
        return Registers_Per_Cluster*cluster + Clusters_Start + 2;
    }

    static constexpr unsigned Count_Register(unsigned cluster) {
        return Registers_Per_Cluster*cluster + Clusters_Start + 3;
    }
};

#endif
