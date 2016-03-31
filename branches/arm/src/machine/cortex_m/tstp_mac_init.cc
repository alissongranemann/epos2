#include <tstp.h>
#include <ic.h>

__BEGIN_SYS

template<>
TSTP_MAC::TSTP_MAC(CC2538_PHY * phy, DMA_Buffer * dma_buf) : _rx_cur(0), _tx_cur(0), _phy(phy) { // TODO: Polymorphic PHY
    assert(MTU <= CC2538_PHY::MTU);

    auto log = dma_buf->log_address();

    for (auto i = 0u; i < TX_BUFS; ++i) {
        _tx_buffer[i] = new (log) Buffer(0);
        log += sizeof(Buffer);
    }

    for (auto i = 0u; i < RX_BUFS; ++i) {
        _rx_buffer[i] = new (log) Buffer(0);
        log += sizeof(Buffer);
    }
    _phy->rx(); // TODO: remove
}

template<>
void TSTP_MAC::init<CC2538_PHY>(unsigned int unit)
{
    IO_Irq irq = 26;
    DMA_Buffer * dma_buf = new (SYSTEM) DMA_Buffer(DMA_BUFFER_SIZE);
    CC2538_PHY * phy = new (SYSTEM) CC2538_PHY();
    _macs[unit].interrupt = IC::irq2int(irq);
    _macs[unit].mac = new (SYSTEM) TSTP_MAC(phy, dma_buf); // TODO: polymorphic PHY

    // Install interrupt handler
    IC::int_vector(_macs[unit].interrupt, &int_handler<CC2538_PHY>);
    // Enable interrupts for device
    IC::enable(irq);
}

__END_SYS
