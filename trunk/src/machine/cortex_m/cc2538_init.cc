// EPOS Cortex_M TI CC2538 IEEE 802.15.4 NIC Mediator Initialization

#include <system/config.h>
#ifndef __no_networking__

#include <system.h>
#include <machine/cortex_m/machine.h>
#include <machine/cortex_m/cc2538.h>

__BEGIN_SYS

CC2538::CC2538(unsigned int unit, IO_Irq irq, DMA_Buffer * dma_buf) 
    : _unit(unit), _irq(irq), _dma_buf(dma_buf), _rx_cur(0)
{
    db<CC2538>(TRC) << "CC2538(unit=" << unit << ",irq=" << irq << ",dma=" << dma_buf << ")" << endl;

    // Allocate RX buffers
    auto log = _dma_buf->log_address();
    for (auto i = 0u; i < RX_BUFS; ++i) {
        _rx_buffer[i] = new (log) Buffer(0);
        log += sizeof(Buffer);
    }

    // Set Address
    ffsm(SHORT_ADDR0) = _address[0];
    ffsm(SHORT_ADDR1) = _address[1];
    _address[0] = ffsm(SHORT_ADDR0);
    _address[1] = ffsm(SHORT_ADDR1);

    // Enable frame filtering
    xreg(FRMFILT0) |= FRAME_FILTER_EN;
    xreg(FRMFILT1) &= ~ACCEPT_FT2_ACK; // ACK frames are handled only when expected

    // Enable automatic source address matching
    xreg(SRCMATCH) |= SRC_MATCH_EN;

	// Enable auto-CRC
	xreg(FRMCTRL0) |= AUTO_CRC;

    channel(15);

    // Enable auto ACK
    xreg(FRMCTRL0) |= AUTO_ACK;

    // Reset statistics
    reset();

    xreg(FRMCTRL1) |= SET_RXENMASK_ON_TX; // Enter receive mode after TX

    // Enable useful device interrupts
    // WARNING: do not enable INT_TXDONE, because _send_and_wait handles it
    xreg(RFIRQM0) = INT_FIFOP;
    xreg(RFIRQM1) = 0;
    xreg(RFERRM) = 0;

    // Issue the listen command
    rx();
}


void CC2538::init(unsigned int unit)
{
    db<Init, CC2538>(TRC) << "Radio::init(unit=" << unit << ")" << endl;

    // Allocate a DMA Buffer for init block, rx and tx rings
    DMA_Buffer * dma_buf = new (SYSTEM) DMA_Buffer(DMA_BUFFER_SIZE);

    IO_Irq irq = IC::IRQ_RFTXRX;

    // Initialize the device
    CC2538 * dev = new (SYSTEM) CC2538(unit, irq, dma_buf);

    // Register the device
    _devices[unit].device = dev;
    _devices[unit].interrupt = IC::irq2int(irq);
    
    // Install interrupt handler
    IC::int_vector(_devices[unit].interrupt, &int_handler);

    // Enable interrupts for device
    IC::enable(irq);
}

__END_SYS

#endif
