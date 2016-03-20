#ifndef __ia32_mmu_aux_h
#define __ia32_mmu_aux_h

#include <cpu.h>

__BEGIN_SYS

class IA32_MMU_Aux
{
public:
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Phy_Addr Phy_Addr;
    typedef CPU::Reg32 Reg32;

public:
    /*! Returns the physical address given a logical address
     * (linear address in IA32 terminology).
     * Takes into account the current page directory of the CPU that executes
     * this code.
     * Returns on the output parameter out_page_frame_present whether there is
     * a mapping or not regarding the informed logical address.
     * */
    static unsigned long physical_address(unsigned long log_addr, unsigned long * out_page_frame_present);

    /*! Prints the memory mapping (page resolution) for the current page
     * directory of the CPU that executes this code. */
    static void dump_memory_mapping();

    /// Prints the memory mapping of an object starting at **log_addr** having size **size** (specified in bytes)
    /*! Prints the memory mapping (page resolution) from the page regarding the
     * informed logical address to the last page where the object lies
     * (**size**, specified in bytes, is used to determined that).
     * This method takes into account the current page directory of the CPU that
     * executes this code.
     * This method stops with an error on the first non mapped logical address.
     * If **dump** is set to false, this method only checks whether the mappings
     * are valid without printing them.
     * */
    static void dump_memory_mapping(unsigned long log_addr, unsigned long size, bool dump = true);

    static void check_memory_mapping();

private:
    static void check_mapping(const char * component_name, unsigned long log_addr, unsigned long expected_phy_addr);
};

__END_SYS

#endif
