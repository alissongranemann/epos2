#include <tstp.h>
#include <ic.h>

__BEGIN_SYS

template<>
One_Hop_MAC::One_Hop_MAC(CC2538_PHY * phy, DMA_Buffer * dma_buf) : _rx_cur(0), _tx_cur(0), _phy(phy) { // TODO: Polymorphic PHY
    db<One_Hop_MAC>(TRC) << "One_Hop_MAC::One_Hop_MAC(phy = " << phy << ", dma = " << dma_buf << ")" << endl;
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
    _phy->rx();
}

template<>
TSTP_MAC::TSTP_MAC(CC2538_PHY * phy, DMA_Buffer * dma_buf) : _tx_pin('c',2,GPIO::OUTPUT), _rx_pin('c',3,GPIO::OUTPUT), 
    _check_tx_schedule(this), _rx_mf(this), _rx_data(this), _tx(this), _cca(this), _rx_cur(0), _tx_cur(0),  _phy(phy), _receiving_data_id(0), _tx_pending(0), _sending_microframe(0), _last_fwd_id(0xffff), _last_backoff(0) { // TODO: Polymorphic PHY
    db<TSTP_MAC>(TRC) << "TSTP_MAC::TSTP_MAC(phy = " << phy << ", dma = " << dma_buf << ")" << endl;
    assert(MTU <= CC2538_PHY::MTU);

    auto log = dma_buf->log_address();

    for (auto i = 0u; i < TX_BUFS; ++i) {
        auto el = new TX_Schedule::Element(_tx_buffer[i]);
        _tx_buffer[i] = new (log) Buffer(el);
        log += sizeof(Buffer);
    }

    for (auto i = 0u; i < RX_BUFS; ++i) {
        _rx_buffer[i] = new (log) Buffer(0);
        log += sizeof(Buffer);
    }
    _phy->off();
    _phy->channel(Config::DEFAULT_CHANNEL);
}

template<>
void TSTP::MAC::init<CC2538_PHY>(unsigned int unit)
{
    db<TSTP::MAC>(TRC) << "TSTP::MAC::init<CC2538_PHY>(unit = " << unit << ")" << endl;
    IO_Irq irq = 26;
    DMA_Buffer * dma_buf = new (SYSTEM) DMA_Buffer(DMA_BUFFER_SIZE);
    CC2538_PHY * phy = new (SYSTEM) CC2538_PHY();
    _macs[unit].interrupt = IC::irq2int(irq);
    _macs[unit].mac = new (SYSTEM) TSTP::MAC(phy, dma_buf); // TODO: polymorphic PHY

    // Install interrupt handler
    IC::int_vector(_macs[unit].interrupt, &int_handler<CC2538_PHY>);
    // Enable interrupts for device
    IC::enable(irq);
}

__END_SYS
