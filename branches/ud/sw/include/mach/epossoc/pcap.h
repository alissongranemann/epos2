// EPOS EPOSSoC PCAP Mediator

#ifndef __epossoc_pcap_h
#define __epossoc_pcap_h

#include <pcap.h>

__BEGIN_SYS

// The PCAP is being acessed through the AXI master interface in Zynq's PL by
// Plasma.
class EPOSSOC_PCAP: public PCAP_Common
{
private:
    typedef CPU::Reg32 Reg32;

    enum {
        BASE_ADDRESS = Traits<EPOSSOC_PCAP>::BASE_ADDRESS,
    };

    // Registers
    enum {
        CTRL            = 0x00,
        INT_STS         = 0x0C,
        STATUS          = 0x14,
        DMA_SRC_ADDR    = 0x18,
        DMA_DEST_ADDR   = 0x1C,
        DMA_SRC_LEN     = 0x20,
        DMA_DEST_LEN    = 0x24,
        UNLOCK          = 0x34,
        MCTRL           = 0x80
    };

    // CTRL bits
    enum {
        PCAP_RATE_EN    = 25,
        PCAP_MODE       = 26,
        PCAP_PR         = 27
    };

    // INT_STS bits
    enum {
        IXR_D_P_DONE = 12
    };

    // STATUS bits
    enum {
        DMA_CMD_Q_F = 0,
        PCFG_INIT   = 4
    };

    // MCTRL bits
    enum {
        PCAP_LPBK = 4
    };

    // Magical number that unlocks the Device Configuration Interface
    enum {
        UNLOCK_DATA = 0x757BDF0D
    };

    // Invalid DMA address
    enum {
        DMA_INVALID_ADDRESS = 0xFFFFFFFF
    };

public:
    EPOSSOC_PCAP() {
        // Unlock the Device Configuration Interface
        unlock(UNLOCK_DATA);

        // Enable the PCAP interface for partial reconfiguration
        ctrl(ctrl() | (1<<PCAP_MODE) | (1<<PCAP_PR));
    }

    ~EPOSSOC_PCAP(){ }

    // Transfer bitstream from DDR memory into FPGA fabric in non secure mode.
    // bitstream_len is the number of 32 bit words to be transfered.
    void transfer(Reg32 bitstream_addr, Reg32 bitstream_len) {
        // Clear DMA and PCAP Done Interrupts
        int_sts(1<<IXR_D_P_DONE);

        // Check if DMA command queue is full
        if(status() & (1<<DMA_CMD_Q_F)) {
            db<PCAP>(WRN) << "DMA's command queue is full" << endl;
        }

        // Check whether the FPGA fabric is in initialized state
        if((status() & (1<<PCFG_INIT)) == 0) {
            db<PCAP>(WRN) << "PL is not ready to receive PCAP data" << endl;
        }

        // Clear internal PCAP loopback
        mctrl(mctrl() & ~(1<<PCAP_LPBK));

        // Clear QUARTER_PCAP_RATE_EN bit so that the PCAP data is transmitted
        // every clock cycle
        ctrl(ctrl() & ~(1<<PCAP_RATE_EN));

        // Initiates the DMA transfer
        dma_src_addr(bitstream_addr);
        dma_dest_addr(DMA_INVALID_ADDRESS);
        dma_src_len(bitstream_len);
        dma_dest_len(0);

        // Poll PCAP Done Interrupt
        while(!(int_sts() & (1<<IXR_D_P_DONE)));;
    };

private:
    static Reg32 ctrl() { return (*(volatile Reg32 *)(BASE_ADDRESS + CTRL)); }
    static void ctrl(Reg32 value) { *(volatile Reg32 *)(BASE_ADDRESS + CTRL) = value; }

    static Reg32 int_sts() { return (*(volatile Reg32 *)(BASE_ADDRESS + INT_STS)); }
    static void int_sts(Reg32 value) { *(volatile Reg32 *)(BASE_ADDRESS + INT_STS) = value; }

    static Reg32 status() { return (*(volatile Reg32 *)(BASE_ADDRESS + STATUS)); }

    static void dma_src_addr(Reg32 value) { *(volatile Reg32 *)(BASE_ADDRESS + DMA_SRC_ADDR) = value; }
    static Reg32 dma_src_addr() { return (*(volatile Reg32 *)(BASE_ADDRESS + DMA_SRC_ADDR)); }

    static void dma_dest_addr(Reg32 value) { *(volatile Reg32 *)(BASE_ADDRESS + DMA_DEST_ADDR) = value; }
    static Reg32 dma_dest_addr() { return (*(volatile Reg32 *)(BASE_ADDRESS + DMA_DEST_ADDR)); }

    static void dma_src_len(Reg32 value) { *(volatile Reg32 *)(BASE_ADDRESS + DMA_SRC_LEN) = value; }
    static Reg32 dma_src_len() { return (*(volatile Reg32 *)(BASE_ADDRESS + DMA_SRC_LEN)); }

    static void dma_dest_len(Reg32 value) { *(volatile Reg32 *)(BASE_ADDRESS + DMA_DEST_LEN) = value; }
    static Reg32 dma_dest_len() { return (*(volatile Reg32 *)(BASE_ADDRESS + DMA_DEST_LEN)); }

    static void unlock(Reg32 value) { *(volatile Reg32 *)(BASE_ADDRESS + UNLOCK) = value; }

    static Reg32 mctrl() { return (*(volatile Reg32 *)(BASE_ADDRESS + MCTRL)); }
    static void mctrl(Reg32 value) { *(volatile Reg32 *)(BASE_ADDRESS + MCTRL) = value; }
};

__END_SYS

#endif
