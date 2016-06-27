// EPOS PC FPGA Mediator Declarations

#ifndef __pc_fpga_h
#define __pc_fpga_h

#include <cpu.h>
#include <ic.h>
#include <fpga.h>
#include <tsc.h>

__BEGIN_SYS

class XAP1052
{
protected:
    // Type Imports
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Phy_Addr Phy_Addr;
    typedef CPU::IO_Port IO_Port;
    typedef CPU::IO_Irq IO_Irq;
    typedef MMU::DMA_Buffer DMA_Buffer;

    // Default values
    static const unsigned int WRITE_DMA_TLP_SIZE = 0x20;
    static const unsigned int WRITE_DMA_TLP_COUNT = 0x20;
    static const unsigned int WRITE_DMA_TLP_PATTERN = 0xdeadbeef;
    static const unsigned int READ_DMA_TLP_SIZE = 0x20;
    static const unsigned int READ_DMA_TLP_COUNT = 0x20;
    static const unsigned int READ_DMA_TLP_PATTERN = 0xfeedbeef;


    // PCI ID
    static const unsigned int PCI_VENDOR_ID = 0x10ee;
    static const unsigned int PCI_DEVICE_ID = 0x6024;
    static const unsigned int PCI_REG_CTRL = 0;

public:
    // Offsets from base I/O address (PCI region 0)
    enum {
        DCSR       = 0x00,
        DDMACR     = 0x04,
        WDMATLPA   = 0x08,
        WDMATLPS   = 0x0c,
        WDMATLPC   = 0x10,
        WDMATLPP   = 0x14,
        RDMATLPP   = 0x18,
        RDMATLPA   = 0x1c,
        RDMATLPS   = 0x20,
        RDMATLPC   = 0x24,
        WDMAPERF   = 0x28,
        RDMAPERF   = 0x2c,
        RDMASTAT   = 0x30,
        NRDCOMP    = 0x34,
        RCOMPDSIZW = 0x38,
        DLWSTAT    = 0x3c,
        DLTRSSTAT  = 0x40,
        DMISCCONT  = 0x44
    };


public:
    static void reset();

    static void run();

    static void wait_for_transaction();

    static void report();

    static void print_configuration();


protected:
    static Reg32 read(unsigned long offset) {
        Reg32 * aux = _base + offset;

        return *aux;
    }

    static void write(unsigned long offset, Reg32 value) {
        Reg32 * aux = _base + offset;

        *aux = value;
    }

    static void print_bmd_registers();

    static void _delay();
    static void _print_dma_buffer(DMA_Buffer * buffer);

protected:
    static Log_Addr _base;
    static DMA_Buffer * _dma_write_buffer; /* Device will write in this buffer. User will read from this buffer. */
    static DMA_Buffer * _dma_read_buffer; /* Device will read from this buffer. User will write on this buffer. */
    static int _runs;
};


// PC FPGA
class PC_FPGA: private FPGA_Common, private XAP1052
{
    friend class PC;

public:
    // Traits
    static const unsigned int DMA_BUFFER_SIZE = Traits<FPGA>::DMA_BUFFER_SIZE;

public:
    typedef XAP1052 Engine;

public:
    PC_FPGA() {}

    static void reset() { Engine::reset(); }

    static void run() { Engine::run(); }

    static void report() { Engine::report(); }

    static void wait_for_transaction() { Engine::wait_for_transaction(); }

    static void print_configuration() { Engine::print_configuration(); }

private:
    static void int_handler(const IC::Interrupt_Id & interrupt);

    static void init();
};

__END_SYS

#endif
