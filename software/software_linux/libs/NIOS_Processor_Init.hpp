#ifndef NIOS_INIT_HPP
#define NIOS_INIT_HPP

#include <cstdint>

namespace NIOS_Processor_Init {

    /*
     * Program the NIOS Processor
     * sdram_datfile - name of the .dat file with the program data
     * sdram_base_addr - the base address of the sdram, get it from arm_system.h
     * sdram_span - span of the sdram, also in arm_system.h
     * mm_reset_base_addr - mm_reset base addr
     * mm_reset_span - mm_reset span
     *
     */
    int program_nios(const char *sdram_datfile, 
            uintptr_t sdram_base_addr, unsigned sdram_span,
            uintptr_t mm_reset_base_addr, unsigned mm_reset_span);

};


#endif // NIOS_INIT_HPP
