#ifndef PIXELCLUSTER_HPP
#define PIXELCLUSTER_HPP
#include <stdint.h>
#include <functional>
#include <iostream>
#include <array>
#include <ratio>
#include <sys/alt_irq.h>
#include <sys/alt_alarm.h>

class PixelCluster {
public:
    static constexpr unsigned Video_Width = 320;
    static constexpr unsigned Video_Height = 240;
    static constexpr unsigned N_Clusters = 4;

    struct ClusterData {
        unsigned avg_x, avg_y; // center
        unsigned min_x, min_y; // bounding box
        unsigned max_x, max_y;
        unsigned count;         // # of pixels

        constexpr ClusterData():
            avg_x(0), avg_y(0), min_x(0), min_y(0), 
            max_x(0), max_y(0), count(0) {}

        // Helper copy constructor which scales the cluster
        template <std::intmax_t N, std::intmax_t D>
        constexpr ClusterData(const ClusterData &d, std::ratio<N, D> r) :
            avg_x(d.avg_x * r.num / r.den), avg_y(d.avg_y * r.num / r.den),
            min_x(d.min_x * r.num / r.den), min_y(d.min_y * r.num / r.den),
            max_x(d.max_x * r.num / r.den), max_y(d.max_y * r.num / r.den) {}

        template <std::intmax_t N, std::intmax_t D>
        void scale(std::ratio<N, D> r) {
            avg_x = avg_x * r.num / r.den;
            avg_y = avg_y * r.num / r.den;
            min_x = min_x * r.num / r.den;
            min_y = min_y * r.num / r.den;
            max_x = max_x * r.num / r.den;
            max_y = max_y * r.num / r.den;
        }
    };

    typedef std::array<ClusterData, N_Clusters> NClusterData;

    void startIRQ();
    void stopIRQ();
    bool irq_overflow();

    void reset();

    // Get average period at which the isr is triggered
    // reset will reset the period stats
    unsigned isr_period_ms(bool reset=true);

    bool is_done();
    void ack();

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

    // scale = amount to multiply the output x-y values by
    PixelCluster(uintptr_t base, int ic_id, int irq_id, NClusterData &data);

private:
    uint32_t * const m_base;
    const alt_u32 m_ic_id;
    const alt_u32 m_irq_id;

    NClusterData &m_last_data;

    unsigned m_last_isr_ticks;
    unsigned m_accumlate_isr_ticks;
    unsigned m_accumulate_count;

    bool m_acked;
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

// Print overloads for clusters
inline std::ostream& operator<<(std::ostream& os, const PixelCluster::ClusterData& c)  {  
    os << "Cluster:\n"
        << "AVG: " << c.avg_x << ',' << c.avg_y << '\n'
        << "MIN: " << c.min_x << ',' << c.min_y << '\n'
        << "MAX: " << c.max_x << ',' << c.max_y << '\n';
    return os;  
}  

inline std::ostream& operator<<(std::ostream& os, const PixelCluster::NClusterData& c)  {  
    os << "Begin Cluster Array\n";
    for (unsigned i = 0; i < c.size(); i++) {
        os << i << ':' << c[i];
    }
    os << "\nEnd Cluster Array\n";
    return os;  
}  

#endif
