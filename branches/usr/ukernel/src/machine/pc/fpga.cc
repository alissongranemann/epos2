// EPOS PC FPGA Mediator Implementation

#include <machine/pc/machine.h>
#include <machine/pc/ic.h>
#include <machine/pc/fpga.h>

__BEGIN_SYS

// Class attributes
CPU::Log_Addr PC_FPGA::Engine::_base;
MMU::DMA_Buffer * PC_FPGA::Engine::_dma_write_buffer;
MMU::DMA_Buffer * PC_FPGA::Engine::_dma_read_buffer;
int PC_FPGA::Engine::_runs = 0;

// Class methods
void PC_FPGA::int_handler(const IC::Interrupt_Id & i)
{

}


/// XAP1052

void XAP1052::reset()
{
    /// Set by xbmd driver at XPCIe_InitCard
    write(DCSR, read(DCSR) | 0x1);              // Reset Device
    write(DCSR, read(DCSR) & 0xfffffffe);       // Make Active

    write(WDMATLPA, _dma_write_buffer->phy_address());   // Write DMA TLP Address register with starting address
    write(WDMATLPS, WRITE_DMA_TLP_SIZE);                 // Write DMA TLP Size register with default value (number of dwords). Supported range: [0x1, 0x1fff]
    write(WDMATLPC, WRITE_DMA_TLP_COUNT);                // Write DMA TLP Count register with default value. Supported range: [0x1, 0xffff]
    write(WDMATLPP, WRITE_DMA_TLP_PATTERN);              // Write DMA TLP Pattern register with default value (0x0)

    write(RDMATLPP, READ_DMA_TLP_PATTERN);
    write(RDMATLPA, _dma_read_buffer->phy_address());
    write(RDMATLPS, READ_DMA_TLP_SIZE);
    write(RDMATLPC, READ_DMA_TLP_COUNT);

    _runs = 0;
    /// ----
}


void XAP1052::run()
{
    if ((read(DCSR) >> 24) != 0x14) { // If family != Virtex-6: error!
        db<PC_FPGA>(ERR) << "Wrong FPGA family. Probably error on device configuration.";
    }

    if (_dma_write_buffer->phy_address() & 0x3) {
        db<PC_FPGA>(ERR) << "DMA write buffer address must be multiple of 4.";
    }

    if (_dma_read_buffer->phy_address() & 0x3) {
        db<PC_FPGA>(ERR) << "DMA read buffer address must be multiple of 4.";
    }

    /// "1. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    // TODO
    /// ----

    /// "2. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    write(DCSR, read(DCSR) | 0x1);              // Reset Device
    write(DCSR, read(DCSR) & 0xfffffffe);       // Make Active
    /// ----

    /// "3. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    ASM("_3_ioctl_begin:"); // Check whether this is not optimized away
    read(DDMACR);
    ASM("_3_ioctl_end:");
    /// ----

    /// "4. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    write(WDMATLPC, WRITE_DMA_TLP_COUNT);
    /// ----

    /// "5. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    write(WDMATLPS, WRITE_DMA_TLP_SIZE);
    /// ----

    /// "6. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    write(RDMATLPC, READ_DMA_TLP_COUNT);
    /// ----

    /// "7. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    write(RDMATLPS, READ_DMA_TLP_SIZE);
    /// ----

    /// "8. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    write(WDMATLPP, WRITE_DMA_TLP_PATTERN);
    /// ----

    /// "9. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    write(RDMATLPP, READ_DMA_TLP_PATTERN);
    /// ----

    /// "10. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    ASM("_10_ioctl_begin:");
    read(WDMATLPC);
    ASM("_10_ioctl_end:");
    /// ----

    /// "11. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    ASM("_11_ioctl_begin:");
    read(WDMATLPS);
    ASM("_11_ioctl_end:");
    /// ----

    /// "12. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    ASM("_12_ioctl_begin:");
    read(RDMATLPC);
    ASM("_12_ioctl_end:");
    /// ----

    /// "13. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    ASM("_13_ioctl_begin:");
    read(RDMATLPS);
    ASM("_13_ioctl_end:");
    /// ----

    /// "14. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    ASM("_14_ioctl_begin:");
    read(WDMATLPP);
    ASM("_14_ioctl_end:");
    /// ----

    /// "15. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    ASM("_15_ioctl_begin:");
    read(RDMATLPP);
    ASM("_15_ioctl_end:");
    /// ----

    /// "16. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    write(DMISCCONT, 0x0);
    /// ----

    if (read(WDMAPERF) != 0) {
        db<void>(WRN) << "\nWARNING: FPGA DMA NOT OK" << endl;
    }

    /// "17. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    // Starts write (and read) transaction(s)
    write(DDMACR, read(DDMACR) | 0xffffffff); // It seems to work. Review that.
    /// ----

    // Maybe add a delay here.
    wait_for_transaction();

    /// "18. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    ASM("_18_ioctl_begin:");
    read(DDMACR);
    ASM("_18_ioctl_end:");
    /// ----

    /// "19. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    // TODO
    /// ----

    /// "20. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    ASM("_20_ioctl_begin:");
    read(WDMAPERF);
    ASM("_20_ioctl_end:");
    /// ----

    /// "21. ioctl" @ bmd_t::run_xbmd (see Xbmd_Virtex_6_App.rst)
    ASM("_21_ioctl_begin:");
    read(RDMAPERF);
    ASM("_21_ioctl_end:");
    /// ----

     _runs ++;
}


void XAP1052::_delay()
{
    // 84990000L
    for (unsigned long long i = 0; i < 16998L; i++) {
        ASM("nop\n");
    }
}


void XAP1052::wait_for_transaction()
{
    /*
    do {
        _delay();
    } while(! read(WDMAPERF));
    */

    while(! read(WDMAPERF));
}


void XAP1052::report()
{
    _print_dma_buffer(_dma_write_buffer);
    _print_dma_buffer(_dma_read_buffer);
    print_configuration();
    db<void>(WRN) << "\nRuns: " << _runs << endl;
}


/* Prints the first 5 and the last 5 elements of the buffer */
void XAP1052::_print_dma_buffer(DMA_Buffer * buffer)
{
    unsigned long * log_addr = buffer->log_address();
    unsigned long * phy_addr = buffer->phy_address();
    unsigned int buffer_size = PC_FPGA::DMA_BUFFER_SIZE;
    unsigned int size = 5;

    if (buffer == _dma_write_buffer) {
        db<void>(WRN) << "\nWrite ";
    } else {
        db<void>(WRN) << "\nRead ";
    }
    db<void>(WRN) << "buffer: " << endl;
    for (unsigned int i = 0; (i < size) && (i < buffer_size); i++) {
        db<void>(WRN) << reinterpret_cast<void *>(log_addr) << " (" << phy_addr << ") => " << reinterpret_cast<void *>(*log_addr) << endl;
        log_addr ++;
        phy_addr ++;
    }
    db<void>(WRN) << "..." << endl;
    log_addr = buffer->log_address() + (buffer_size) - (size * sizeof(unsigned long *));
    phy_addr = buffer->phy_address() + (buffer_size) - (size * sizeof(unsigned long *));

    db<void>(WRN) << "<log_addr>: " << log_addr << endl;

    for (unsigned int i = buffer_size - size; i < buffer_size; i++) {
        db<void>(WRN) << reinterpret_cast<void *>(log_addr) << " (" << phy_addr << ") => " << reinterpret_cast<void *>(*log_addr) << endl;
        log_addr ++;
        phy_addr ++;
    }
    db<void>(WRN) << "----" << endl;
}

void XAP1052::print_configuration()
{
    print_bmd_registers();
}


void XAP1052::print_bmd_registers()
{
    db<void>(WRN) << "base = " << _base << endl;
    db<void>(WRN) << "DCSR = " << reinterpret_cast<void *>(read(DCSR)) << endl;
    db<void>(WRN) << "DDMACR = " << reinterpret_cast<void *>(read(DDMACR)) << endl;
    db<void>(WRN) << "WDMATLPA = " << reinterpret_cast<void *>(read(WDMATLPA)) << endl;
    db<void>(WRN) << "WDMATLPS = " << reinterpret_cast<void *>(read(WDMATLPS)) << endl;
    db<void>(WRN) << "WDMATLPC = " << reinterpret_cast<void *>(read(WDMATLPC)) << endl;
    db<void>(WRN) << "WDMATLPP = " << reinterpret_cast<void *>(read(WDMATLPP)) << endl;
    db<void>(WRN) << "RDMATLPP = " << reinterpret_cast<void *>(read(RDMATLPP)) << endl;
    db<void>(WRN) << "RDMATLPA = " << reinterpret_cast<void *>(read(RDMATLPA)) << endl;
    db<void>(WRN) << "RDMATLPS = " << reinterpret_cast<void *>(read(RDMATLPS)) << endl;
    db<void>(WRN) << "RDMATLPC = " << reinterpret_cast<void *>(read(RDMATLPC)) << endl;
    db<void>(WRN) << "WDMAPERF = " << reinterpret_cast<void *>(read(WDMAPERF)) << endl;
    db<void>(WRN) << "RDMAPERF = " << reinterpret_cast<void *>(read(RDMAPERF)) << endl;
    db<void>(WRN) << "RDMASTAT = " << reinterpret_cast<void *>(read(RDMASTAT)) << endl;
    db<void>(WRN) << "NRDCOMP = " << reinterpret_cast<void *>(read(NRDCOMP)) << endl;
    db<void>(WRN) << "RCOMPDSIZW = " << reinterpret_cast<void *>(read(RCOMPDSIZW)) << endl;
    db<void>(WRN) << "DLWSTAT = " << reinterpret_cast<void *>(read(DLWSTAT)) << endl;
    db<void>(WRN) << "DLTRSSTAT = " << reinterpret_cast<void *>(read(DLTRSSTAT)) << endl;
    db<void>(WRN) << "DMISCCONT = " << reinterpret_cast<void *>(read(DMISCCONT)) << endl;
}

/// ----

__END_SYS
