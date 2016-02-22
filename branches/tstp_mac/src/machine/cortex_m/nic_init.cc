// EPOS CC2538 IEEE 802.15.4 NIC Mediator Initialization

#include <system/config.h>
#ifndef __no_networking__

#include <system.h>
#include <utility/random.h>
#include <machine/cortex_m/machine.h>
#include "../../../include/machine/cortex_m/cc2538_radio.h"
#include <gpio.h>

__BEGIN_SYS

CC2538::CC2538(unsigned int unit, IO_Irq irq, DMA_Buffer * dma_buf):
    _unit(unit), _irq(irq), _dma_buf(dma_buf), _rx_cur(0), _tx_cur(0) 
{
    _tx_pin = new (SYSTEM) GPIO('d',4,GPIO::OUTPUT);
    _rx_pin = new (SYSTEM) GPIO('d',2,GPIO::OUTPUT);
    _tx_pin->clear();
    _rx_pin->clear();
    db<CC2538>(TRC) << "CC2538(unit=" << unit << ",irq=" << irq << ")" << endl;    
    // TODO: it has been observed that at least the FRMCTRL1 register does not get
    // its value updated on a write if this instruction is executed.
    // Disable clock to the RF CORE module
//     Cortex_M_Model::radio_disable();
//     for(volatile int i=0; i<0x1ffff; i++);

    // Enable clock to the RF CORE module
    Cortex_M_Model::radio_enable();

    // Disable device interrupts
    xreg(RFIRQM0) = 0;
    xreg(RFIRQM1) = 0;

    // Change recommended in the user guide (CCACTRL0 register description)
    xreg(CCACTRL0) = 0xF8;

    // Changes recommended in the user guide (Section 23.15 Register Settings Update)
    xreg(TXFILTCFG) = 0x09;
    xreg(AGCCTRL1) = 0x15;
    ana(IVCTRL) = 0x0b;
    xreg(FSCAL1) = 0x01;


    // Ignore TX underflow to enable writing to TXFIFO through memory
    // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
    // For now, we'll just copy using RFDATA register.
	//xreg(FRMCTRL1) |= IGNORE_TX_UNDERF;

    // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
    // For now, we'll just copy using RFDATA register.
    //_tx_buffer[0] = reinterpret_cast<Buffer*>(TXFIFO);
    //_rx_buffer[0] = reinterpret_cast<Buffer*>(RXFIFO);

    for (auto i = 0u; i < TX_BUFS; ++i) {
        _tx_buffer[i] = new (SYSTEM) Buffer(0);
    }

    auto log = _dma_buf->log_address();

    for (auto i = 0u; i < RX_BUFS; ++i) {
        _rx_buffer[i] = new (log) Buffer(0);
        log += sizeof(Buffer);
    }

    // Disable symbol search completely (will be turned on by listen())
    // TODO: This line caused interrupts to stop and never turn on.
    //xreg(FRMCTRL0) |= (3 * RX_MODE);

    off();

    sfr(RFST) = ISFLUSHTX; // Clear TXFIFO
    sfr(RFST) = ISFLUSHRX; // Clear RXFIFO

    // Set Address
    //ffsm(SHORT_ADDR0) = _address[0];
    //ffsm(SHORT_ADDR1) = _address[1];
    //_address[0] = ffsm(SHORT_ADDR0);
    //_address[1] = ffsm(SHORT_ADDR1);

    // Set PAN ID
    //ffsm(PAN_ID0) = DEFAULT_PAN_ID;
    //ffsm(PAN_ID1) = DEFAULT_PAN_ID >> 8;

    // Make this device a pan coordinator
    //xreg(FRMFILT0) |= PAN_COORDINATOR;

    // Disable frame filtering
    xreg(FRMFILT0) &= ~FRAME_FILTER_EN;
    xreg(FRMFILT1) &= ~ACCEPT_FT2_ACK; // ACK frames are handled only when expected

    // Reset result of source matching (value undefined on reset)
    ffsm(SRCRESINDEX) = 0;

    // Enable automatic source address matching
    xreg(SRCMATCH) |= SRC_MATCH_EN;

    // Set FIFOP threshold to maximum
    xreg(FIFOPCTRL) = 0xff;

    // Set TXPOWER (this is the value Contiki uses by default)
    xreg(TXPOWER) = 0xD5;

	// Enable auto-CRC
	xreg(FRMCTRL0) |= AUTO_CRC;

    set_channel(Traits<CC2538>::DEFAULT_CHANNEL);

	// Disable counting of MAC overflows
	xreg(CSPT) = 0xff;

    // Clear interrupts
    sfr(RFIRQF0) = 0;
    sfr(RFIRQF1) = 0;

    // Clear error flags
    sfr(RFERRF) = 0;

    // Enable useful device interrupts
    xreg(RFIRQM0) = INT_FIFOP;
    xreg(RFIRQM1) = 0;

    xreg(FRMCTRL1) &= ~SET_RXENMASK_ON_TX; // Do not enter receive mode after TX
}


void CC2538::init(unsigned int unit)
{
    db<Init, CC2538>(TRC) << "CC2538::init(unit=" << unit << ")" << endl;

    // Allocate a DMA Buffer for init block, rx and tx rings
    DMA_Buffer * dma_buf = new (SYSTEM) DMA_Buffer(DMA_BUFFER_SIZE);

    IO_Irq irq = 26;

    // Initialize the device
    CC2538 * dev = new (SYSTEM) CC2538(unit, irq, dma_buf);

    // Register the device
    _devices[unit].interrupt = IC::irq2int(irq);
    _devices[unit].device = dev;

    // Install interrupt handler
    IC::int_vector(_devices[unit].interrupt, &int_handler);
    // Enable interrupts for device
    IC::enable(irq);
}

__END_SYS

#endif
