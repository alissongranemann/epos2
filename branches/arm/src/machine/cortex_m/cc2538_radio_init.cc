// EPOS CC2538 IEEE 802.15.4 NIC Mediator Initialization

#include <system/config.h>
#ifndef __no_networking__

#include <system.h>
#include <utility/random.h>
#include <machine/cortex_m/machine.h>
#include "../../../include/machine/cortex_m/cc2538_radio.h"

__BEGIN_SYS

template<typename MAC>
CC2538<MAC>::CC2538(unsigned int unit, IO_Irq irq, DMA_Buffer * dma_buf):
    _unit(unit), _irq(irq), _dma_buf(dma_buf), _rx_cur(0), _tx_cur(0)
{
    db<CC2538>(TRC) << "CC2538(unit=" << unit << ",irq=" << irq << ")" << endl;

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

    auto log = _dma_buf->log_address();

    for (auto i = 0u; i < TX_BUFS; ++i) {
        _tx_buffer[i] = new (log) Buffer(0);
        log += sizeof(Buffer);
    }
    for (auto i = 0u; i < RX_BUFS; ++i) {
        _rx_buffer[i] = new (log) Buffer(0);
        log += sizeof(Buffer);
    }

    sfr(RFST) = ISFLUSHTX; // Clear TXFIFO
    sfr(RFST) = ISFLUSHRX; // Clear RXFIFO

    // Set Address
    ffsm(SHORT_ADDR0) = _address[0];
    ffsm(SHORT_ADDR1) = _address[1];
    _address[0] = ffsm(SHORT_ADDR0);
    _address[1] = ffsm(SHORT_ADDR1);

    // Set PAN ID
    //ffsm(PAN_ID0) = DEFAULT_PAN_ID;
    //ffsm(PAN_ID1) = DEFAULT_PAN_ID >> 8;

    // Make this device a pan coordinator
    //xreg(FRMFILT0) |= PAN_COORDINATOR;

    // Enable frame filtering
    xreg(FRMFILT0) |= FRAME_FILTER_EN;
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

    rx_mode(RX_MODE_NORMAL);

    channel(Traits<CC2538<MAC>>::DEFAULT_CHANNEL);

	// Disable counting of MAC overflows
	xreg(CSPT) = 0xff;

    // Enable auto ACK
    if(Traits<CC2538<MAC>>::ACK)
        xreg(FRMCTRL0) |= AUTO_ACK;

    // Clear interrupts
    sfr(RFIRQF0) = 0;
    sfr(RFIRQF1) = 0;

    // Clear error flags
    sfr(RFERRF) = 0;

    // Reset statistics
    reset();

    if(Traits<CC2538<MAC>>::auto_listen)
    {
 	    xreg(FRMCTRL1) |= SET_RXENMASK_ON_TX; // Enter receive mode after TX

        // Enable useful device interrupts
        // WARNING: do not enable INT_TXDONE, because _send_and_wait handles it
        xreg(RFIRQM0) = INT_FIFOP;
        xreg(RFIRQM1) = 0;

        // Enable clock to the RF CORE module
        // Cortex_M_Model::radio_enable(); // already done

        // Issue the listen command
        sfr(RFST) = ISRXON;
    }
    else
    {
	    xreg(FRMCTRL1) &= ~SET_RXENMASK_ON_TX; // Do not enter receive mode after TX

        // Enable clock to the RF CORE module
        // Cortex_M_Model::radio_enable(); // already done
    }
}

template<typename MAC>
void CC2538<MAC>::init(unsigned int unit)
{
    db<Init, CC2538>(TRC) << "CC2538::init(unit=" << unit << ")" << endl;

    // Allocate a DMA Buffer for init block, rx and tx rings
    DMA_Buffer * dma_buf = new (SYSTEM) DMA_Buffer(DMA_BUFFER_SIZE);

    IO_Irq irq = 26;

    // Initialize the device
    CC2538<MAC> * dev = new (SYSTEM) CC2538<MAC>(unit, irq, dma_buf);

    // Register the device
    _devices[unit].interrupt = IC::irq2int(irq);
    _devices[unit].device = dev;

    // Install interrupt handler
    IC::int_vector(_devices[unit].interrupt, &int_handler);
    // Enable interrupts for device
    IC::enable(irq);
}

template class CC2538<TSTP_MAC>;
template class CC2538<IEEE802_15_4>;

__END_SYS

#endif
