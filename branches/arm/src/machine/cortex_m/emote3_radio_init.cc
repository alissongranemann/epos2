// EPOS eMote3 IEEE802.15.4 Radio Mediator Initialization

#include <system.h>
#include <machine/cortex_m/machine.h>
#include <machine/cortex_m/emote3_radio.h>

__BEGIN_SYS

eMote3_Radio::eMote3_Radio(unsigned int unit, IO_Irq irq)
{
    db<eMote3_Radio>(TRC) << "eMote3_Radio(unit=" << unit << ",irq=" << irq << ")" << endl;
    // TODO: it has been observed that at least the FRMCTRL1 register does not get
    // its value updated on a write if this instruction is executed.
    // Disable clock to the RF CORE module
    Cortex_M_Model::radio_disable();

    _tx_cur = _rx_cur = 0;
    _unit = unit;
    _irq = irq;

    // Ignore TX underflow to enable writing to TXFIFO through memory
    // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
    // For now, we'll just copy using RFDATA register.
	//xreg(FRMCTRL1) |= IGNORE_TX_UNDERF;

    // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
    // For now, we'll just copy using RFDATA register.
    //_tx_buffer[0] = reinterpret_cast<Buffer*>(TXFIFO);
    //_rx_buffer[0] = reinterpret_cast<Buffer*>(RXFIFO);
    _tx_buffer[0] = new (SYSTEM) Buffer(0);
    _rx_buffer[0] = new (SYSTEM) Buffer(0);

    // Disable symbol search completely (will be turned on by listen())
    // TODO: This line causes interrupts to stop and never turn on.
    //xreg(FRMCTRL0) |= (3 * RX_MODE);

    sfr(RFST) = ISFLUSHTX; // Clear TXFIFO
    sfr(RFST) = ISFLUSHRX; // Clear RXFIFO

    // Set PAN ID
    //ffsm(PAN_ID0) = DEFAULT_PAN_ID;
    //ffsm(PAN_ID1) = DEFAULT_PAN_ID >> 8;

    // Make this device a pan coordinator
    //xreg(FRMFILT0) |= PAN_COORDINATOR;

    // Enable frame filtering
    xreg(FRMFILT0) |= FRAME_FILTER_EN;

    // Reset result of source matching (value undefined on reset)
    ffsm(SRCRESINDEX) = 0;

    // Enable automatic source address matching
    xreg(SRCMATCH) |= SRC_MATCH_EN;

    // Set FIFOP threshold to maximum
    xreg(FIFOPCTRL) = 0xff;

	// Enable auto-CRC
	xreg(FRMCTRL0) |= AUTO_CRC;

    if(Traits<eMote3_Radio>::auto_listen)
	    xreg(FRMCTRL1) |= SET_RXENMASK_ON_TX; // Enter receive mode after TX
    else
	    xreg(FRMCTRL1) &= ~SET_RXENMASK_ON_TX; // Do not enter receive mode after TX

    set_channel(11);

	// Disable counting of MAC overflows
	xreg(CSPT) = 0xff;

    // Disable device interrupts (enabled on listen())
    xreg(RFIRQM0) = 0;
    xreg(RFIRQM1) = 0;

    // Clear interrupts
    sfr(RFIRQF0) = 0;
    sfr(RFIRQF1) = 0;

    // Clear error flags
    sfr(RFERRF) = 0;

    // Reset statistics
    reset();

    // Enable clock to the RF CORE module
    Cortex_M_Model::radio_enable();

    if(Traits<eMote3_Radio>::auto_listen)
    {
        // Enable device interrupts
        xreg(RFIRQM0) = ~0;
        xreg(RFIRQM1) = ~0;

        // Issue the listen command
        sfr(RFST) = ISRXON; 
    }
}


void eMote3_Radio::init(unsigned int unit)
{
    db<Init, eMote3_Radio>(TRC) << "eMote3_Radio::init(unit=" << unit << ")" << endl;

    // Allocate a DMA Buffer for init block, rx and tx rings
    //DMA_Buffer * dma_buf = new (SYSTEM) DMA_Buffer(DMA_BUFFER_SIZE);

    IO_Irq irq = 26;

    // Initialize the device
    eMote3_Radio * dev = new (SYSTEM) eMote3_Radio(unit, irq);

    // Register the device
    _devices[unit].interrupt = IC::irq2int(irq);
    _devices[unit].device = dev;
    
    // Install interrupt handler
    IC::int_vector(_devices[unit].interrupt, &int_handler);
    // Enable interrupts for device
    IC::enable(irq);
}

__END_SYS
