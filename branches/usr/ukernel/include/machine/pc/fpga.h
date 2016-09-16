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
    static const unsigned int DMA_OPERATION = Traits<FPGA>::DMA_OPERATION;
    static const bool USE_INTERRUPTS = Traits<FPGA>::USE_INTERRUPTS;
    static const unsigned int WRITE_DMA_TLP_SIZE = Traits<FPGA>::WRITE_DMA_TLP_SIZE;
    static const unsigned int WRITE_DMA_TLP_COUNT = Traits<FPGA>::WRITE_DMA_TLP_COUNT;
    static const unsigned int WRITE_DMA_TLP_PATTERN = 0xdeadbeef;
    static const unsigned int READ_DMA_TLP_SIZE = 0x20;
    static const unsigned int READ_DMA_TLP_COUNT = 0x20;
    static const unsigned int READ_DMA_TLP_PATTERN = 0xfeedbeef;
    static const unsigned int TRANSFERENCE_SIZE = (DMA_OPERATION == WRITE) ? (WRITE_DMA_TLP_SIZE * WRITE_DMA_TLP_COUNT * sizeof(void*)) : (READ_DMA_TLP_SIZE * READ_DMA_TLP_COUNT * sizeof(void*));
    static const bool AUTO_RESTART_DMA_TRANSACTIONS = Traits<FPGA>::AUTO_RESTART_DMA_TRANSACTIONS;

    static const unsigned int WRITE_DMA_PERF_SIZE = 1000;

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

    // OR-masks for setting bits of the DDMACR register
    enum {
        DDMACR_WRITE_DMA_START                                      = 0x00000001, // Bit[0]
        DDMACR_STOP_WRITE                                           = 0x00000002, // Bit[1]
        DDMACR_WRITE_DMA_RELAXED_ORDERING                           = 0x00000020, // Bit[5]
        DDMACR_WRITE_DMA_NO_SNOOP                                   = 0x00000040, // Bit[6]
        DDMACR_WRITE_DMA_INTERRUPT_DISABLE                          = 0x00000080, // Bit[7]
        DDMACR_READ_DMA_START                                       = 0x00010000, // Bit[16]
        DDMACR_READ_DMA_RELAXED_ORDERING                            = 0x00200000, // Bit[21]
        DDMACR_READ_DMA_NO_SNOOP                                    = 0x00400000, // Bit[22]
        DDMACR_READ_DMA_DONE_INTERRUPT_DISABLE                      = 0x00800000  // Bit[23]
    };
    // AND-masks for reading read-only bits of the DDMACR register
    enum {
        DDMACR_WRITE_DMA_DONE                                       = 0x00000100, // Bit[8]
        DDMACR_READ_DMA_DONE                                        = 0x01000000, // Bit[24]
        DDMACR_READ_DMA_OPERATION_DATA_ERROR                        = 0x80000000  // Bit[31]
    };

    enum {
        DDMACR_FLAGS =      ((DMA_OPERATION == WRITE) ? DDMACR_WRITE_DMA_START : DDMACR_READ_DMA_START)
                        |   DDMACR_WRITE_DMA_RELAXED_ORDERING
                        |   DDMACR_WRITE_DMA_NO_SNOOP
                        |   DDMACR_READ_DMA_RELAXED_ORDERING
                        |   DDMACR_READ_DMA_NO_SNOOP
                        |   ((! USE_INTERRUPTS) ? (DDMACR_WRITE_DMA_INTERRUPT_DISABLE | DDMACR_READ_DMA_DONE_INTERRUPT_DISABLE) : 0)
                        |   ((USE_INTERRUPTS && (DMA_OPERATION == WRITE)) ? DDMACR_READ_DMA_DONE_INTERRUPT_DISABLE : 0)
                        |   ((USE_INTERRUPTS && (DMA_OPERATION == READ)) ? DDMACR_WRITE_DMA_INTERRUPT_DISABLE : 0)
                        |   (AUTO_RESTART_DMA_TRANSACTIONS ? 0 : DDMACR_STOP_WRITE)
    };

public:
    static void reset();

    static void run();

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

    static void wait_for_transaction();

    static void print_bmd_registers();

    static void _delay();
    static void _print_dma_buffer(DMA_Buffer * buffer);
    static void clean_buffer(DMA_Buffer * buffer);

protected:
    static Log_Addr _base;
    static DMA_Buffer * _dma_write_buffer; /* Device will write in this buffer. User will read from this buffer. */
    static DMA_Buffer * _dma_read_buffer; /* Device will read from this buffer. User will write on this buffer. */
    static int _runs;
    static int _waited;
    static Reg32 _wdmaperf[WRITE_DMA_PERF_SIZE]; /* For statistics */
};


// PC FPGA
class PC_FPGA: private FPGA_Common, private XAP1052
{
    friend class PC;

public:
    // Traits
    static const unsigned int DMA_BUFFER_SIZE = Traits<PC_FPGA>::DMA_BUFFER_SIZE;

public:
    typedef XAP1052 Engine;

public:
    PC_FPGA() {}

    static void reset() { Engine::reset(); }

    static void run() { Engine::run(); }

    static void report() { Engine::report(); }

    static void print_configuration() { Engine::print_configuration(); }

private:
    static void int_handler(const IC::Interrupt_Id & interrupt);

    static void init();
};

__END_SYS

#endif
