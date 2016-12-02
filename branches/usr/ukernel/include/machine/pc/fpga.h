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
    static const unsigned int WRITE_DMA_TLP_PATTERN = 0x00000000;
    static const unsigned int READ_DMA_TLP_SIZE = 0x20;
    static const unsigned int READ_DMA_TLP_COUNT = 0x20;
    static const unsigned int READ_DMA_TLP_PATTERN = 0xfeedbeef;
    static const unsigned int TRANSFERENCE_SIZE = (DMA_OPERATION == WRITE) ? (WRITE_DMA_TLP_SIZE * WRITE_DMA_TLP_COUNT * sizeof(void*)) : (READ_DMA_TLP_SIZE * READ_DMA_TLP_COUNT * sizeof(void*));
    static const bool AUTO_RESTART_DMA_TRANSACTIONS = Traits<FPGA>::AUTO_RESTART_DMA_TRANSACTIONS;

    static const unsigned int WRITE_DMA_PERF_SIZE = 1000;

    // PCI ID
    static const unsigned int PCI_VENDOR_ID = 0x10ee;
    static const unsigned int PCI_DEVICE_ID = 0xfada;
    static const unsigned int PCI_REG_CTRL = 0;

public:
    // Offsets from base I/O address (PCI region 0)
    enum {
        DCSR        = 0x00,
        DDMACR      = 0x04,
        WDMATLPA    = 0x08,
        WDMATLPS    = 0x0c,
        WDMATLPC    = 0x10,
        WDMATLPP    = 0x14,
        RDMATLPP    = 0x18,
        RDMATLPA    = 0x1c,
        RDMATLPS    = 0x20,
        RDMATLPC    = 0x24,
        WDMAPERF    = 0x28,
        RDMAPERF    = 0x2c,
        RDMASTAT    = 0x30,
        NRDCOMP     = 0x34,
        RCOMPDSIZW  = 0x38,
        DLWSTAT     = 0x3c,
        DLTRSSTAT   = 0x40,
        DMISCCONT   = 0x44,
        FAKE_DDMACR = 0x50,
        TXMONCR     = 0x54
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

    static void monitor_start();

    static void monitor_stop();

private:
    /*! XAPP1052 was extended to implemented a TX Monitor logic:
     *  whenever TX engine state equals to the one specified by TXMON_REF,
     *  if TXMON_RESET == 0, set TXMON_REACHABLE = 1.
     *  It also increments TXMON_RC "Reached Count" indicating how many times
     *  the TXMON_REF state was reached.
     *  TXMON_RC starts on 0x00 and it is only incremented if it is less than
     *  0xff; otherwise it remains 0xff.
     *  TXMON_RC is only incremented if the previous state of TX engine was not
     *  equal to TXMON_REF and the current state of TX is equal to TXMON_REF,
     *  i.e.: staying in the TXMON_REF state will not cause TXMON_RC to be
     *  incremented.
     *  Setting the TXMON_RESET bit will cause TXMON_REACHABLE to become false
     *  (zero) and TXMON_RC to become zero (0x00).
     *
     * TX Monitor logic is accessed by the Tx Monitor Control Register: TXMONCR,
     * a 32-bit register.
     * */
    enum {
        // AND-masks
        TXMON_REF_MASK          = 0xff000000, // Bit[31:24] (R/W)       {Byte 3}
        TXMON_RC_MASK           = 0x00ff0000, // Bit[23:16] (RO)        {Byte 2}
        TXMON_REACHABLE_MASK    = 0x00008000, // Bit[15] (RO)           {Byte 1}
        TXMON_RESET_MASK        = 0x00000001, // Bit[0] (R/W)           {Byte 0}

        // SHIFTS
        TXMON_REF_SHIFT         = 24,
        TXMON_RC_SHIFT          = 16,
        TXMON_REACHABLE_SHIFT   = 15
    };


    /*! INTERNAL:
     * Tx state-machine states */
    enum {
        BMD_64_TX_CON_STATE    = 0b00000011,
        BMD_64_TX_RST_STATE    = 0b00000001,
        BMD_64_TX_CPLD_QW1     = 0b00000010,
        BMD_64_TX_CPLD_WIT     = 0b00000100,
        BMD_64_TX_MWR_QW1      = 0b00001000,
        BMD_64_TX_MWR64_QW1    = 0b00010000,
        BMD_64_TX_MWR_QWN      = 0b00100000,
        BMD_64_TX_MRD_QW1      = 0b01000000,
        BMD_64_TX_MRD_QWN      = 0b10000000
    };

private:
    static Reg8 txmon_ref() {
        return (read(TXMONCR) & TXMON_REF_MASK) >> TXMON_REF_SHIFT;
    }

    static void txmon_ref(Reg8 state) {
        Reg32 aux = read(TXMONCR) & ~TXMON_REF_MASK;
        write(TXMONCR, aux | (state << TXMON_REF_SHIFT));
    }

    static bool txmon_reachable() {
        return (read(TXMONCR) & TXMON_REACHABLE_MASK) >> TXMON_REACHABLE_SHIFT;
    }

    static void txmon_reset() {
        // Tx monitor resets whenever TXMON_RESET = 1
        write(TXMONCR, read(TXMONCR) | (TXMON_RESET_MASK));
    }

    static void txmon_set() {
        write(TXMONCR, read(TXMONCR) & (~TXMON_RESET_MASK));
    }

    static bool txmon_reseted() {
        return read(TXMONCR) & TXMON_RESET_MASK;
    }

    static unsigned int txmon_rc() {
        return (read(TXMONCR) & TXMON_RC_MASK) >> TXMON_RC_SHIFT;
    }

    static void stop_write_dma_transactions();

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

    static void monitor_start() { Engine::monitor_start(); }

    static void monitor_stop() { Engine::monitor_stop(); }

private:
    static void int_handler(const IC::Interrupt_Id & interrupt);

    static void init();
};

__END_SYS

#endif
