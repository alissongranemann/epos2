// EPOS Cortex_M TI CC2538 IEEE 802.15.4 NIC Mediator Initialization

#include <system/config.h>
#ifndef __no_networking__

#include <system.h>
#include <machine/cortex_m/machine.h>
#include <machine/cortex_m/cc2538.h>

__BEGIN_SYS

CC2538::CC2538(unsigned int unit, IO_Irq irq, DMA_Buffer * dma_buf)
{
    db<CC2538>(TRC) << "CC2538(unit=" << unit << ",irq=" << irq << ",dma=" << dma_buf << ")" << endl;

    _unit = unit;
    _irq = irq;
    _dma_buf = dma_buf;

//    // Distribute the DMA_Buffer allocated by init()
//    Log_Addr log = _dma_buf->log_address();
//    Phy_Addr phy = _dma_buf->phy_address();
//
//    // Rx_Desc Ring
//    _rx_cur = 0;
//    _rx_ring = log;
//    _rx_ring_phy = phy;
//    log += RX_BUFS * align128(sizeof(Rx_Desc));
//    phy += RX_BUFS * align128(sizeof(Rx_Desc));
//
//    // Tx_Desc Ring
//    _tx_cur = 0;
//    _tx_ring = log;
//    _tx_ring_phy = phy;
//    log += TX_BUFS * align128(sizeof(Tx_Desc));
//    phy += TX_BUFS * align128(sizeof(Tx_Desc));
//
//    // Rx_Buffer Ring
//    for(unsigned int i = 0; i < RX_BUFS; i++) {
//        _rx_buffer[i] = new (log) Buffer(&_rx_ring[i]);
//        _rx_ring[i].phy_addr = phy;
//        _rx_ring[i].size = Reg16(-sizeof(Frame)); // 2's comp.
//        _rx_ring[i].misc = 0;
//        _rx_ring[i].status = Desc::OWN; // Owned by NIC
//
//        log += align128(sizeof(Buffer));
//        phy += align128(sizeof(Buffer));
//    }
//
//    // Tx_Buffer Ring
//    for(unsigned int i = 0; i < TX_BUFS; i++) {
//        _tx_buffer[i] = new (log) Buffer(&_tx_ring[i]);
//        _tx_ring[i].phy_addr = phy;
//        _tx_ring[i].size = 0;
//        _tx_ring[i].misc = 0;
//        _tx_ring[i].status = 0; // Owned by host
//
//        log += align128(sizeof(Buffer));
//        phy += align128(sizeof(Buffer));
//    }

    // Reset device
    reset();
}


void CC2538::init(unsigned int unit)
{
    db<Init, CC2538>(TRC) << "Radio::init(unit=" << unit << ")" << endl;

    // Allocate a DMA Buffer for init block, rx and tx rings
    DMA_Buffer * dma_buf = new (SYSTEM) DMA_Buffer(DMA_BUFFER_SIZE);

    IO_Irq irq = 10; // Fixme: this cannot be right

    // Initialize the device
    CC2538 * dev = new (SYSTEM) CC2538(unit, irq, dma_buf);

    // Register the device
    _devices[unit].device = dev;
    _devices[unit].interrupt = IC::irq2int(irq);
    
    // Install interrupt handler
    IC::int_vector(_devices[unit].interrupt, &int_handler);

    // Enable interrupts for device
    IC::enable(_devices[unit].interrupt);
}

__END_SYS

#endif
