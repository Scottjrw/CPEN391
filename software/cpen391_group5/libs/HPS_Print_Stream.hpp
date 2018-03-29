#ifndef HPS_PRINT_STREAM_HPP
#define HPS_PRINT_STREAM_HPP

#include <streambuf>
#include "nios_hps_protocol.hpp"
#include "Hard_Processor_System.hpp"

class HPS_Print_Stream;
/*
 * streambuf implementation for the HPS
 * don't need to instantiate this class directly
 */
class HPS_Print_Buffer : public std::streambuf {
protected:
    virtual int_type overflow(int_type c) {
        if (traits_type::eq_int_type(c, traits_type::eof()) != true) {
            *pptr() = traits_type::to_char_type(c);
            pbump(1);
        }
        sync();

        return traits_type::not_eof(c);
    }

    virtual int sync() {
        if (pbase() != pptr()) {
            m_hps.print(m_buf, pptr() - pbase() - 1);
            m_hps.print_sync();
            setp(pbase(), epptr());
        }

        return 0;
    }

private:
    Hard_Processor_System &m_hps;
    char m_buf[NIOS_HPS_Protocol::NIOS_Print_Data::max_length];

    HPS_Print_Buffer(Hard_Processor_System &hps):
        m_hps(hps)
    {
        setp(m_buf, m_buf + sizeof(m_buf) - 1);
    }

    friend class HPS_Print_Stream;
};

/*
 * ostream using the HPS
 */
class HPS_Print_Stream :
    private virtual HPS_Print_Buffer, 
    public std::ostream {
public:
    HPS_Print_Stream(Hard_Processor_System &hps):
        HPS_Print_Buffer(hps),
        std::ostream(this)
    { }

};

#endif // HPS_PRINT_STREAM_HPP

