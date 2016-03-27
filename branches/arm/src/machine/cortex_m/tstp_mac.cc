#include <tstp_mac.h>
#include <ic.h>

#include "../../../include/machine/cortex_m/cc2538_phy.h"

__BEGIN_SYS

TSTP_MAC::MACS TSTP_MAC::_macs[TSTP_MAC::UNITS];

template<>
TSTP_MAC::TSTP_MAC(CC2538_PHY * phy, unsigned int tx_bufs, unsigned int rx_bufs, DMA_Buffer * dma_buf) : _tx_bufs(tx_bufs), _rx_bufs(rx_bufs), _phy(phy) { // TODO: Polymorphic PHY
    auto log = dma_buf->log_address();

    for (auto i = 0u; i < _tx_bufs; ++i) {
        _tx_buffer[i] = new (log) Buffer(0);
        log += sizeof(Buffer);
    }
    for (auto i = 0u; i < _rx_bufs; ++i) {
        _rx_buffer[i] = new (log) Buffer(0);
        log += sizeof(Buffer);
    }
} 

template <>
void TSTP_MAC::handle_int<CC2538_PHY>()
{
    Reg32 irqrf0 = _phy->sfr(_phy->RFIRQF0);
    Reg32 irqrf1 = _phy->sfr(_phy->RFIRQF1);

    if(irqrf0 & _phy->INT_FIFOP) { // Frame received
        _phy->sfr(_phy->RFIRQF0) &= ~_phy->INT_FIFOP;
        if(_phy->frame_in_rxfifo()) {
            Buffer * buf = 0;

            // NIC received a frame in the RXFIFO, so we need to find an unused buffer for it
            for (auto i = 0u; (i < _rx_bufs) and not buf; ++i) {
                if (_rx_buffer[_rx_cur]->lock()) {
                    db<TSTP_MAC>(INF) << "TSTP_MAC::handle_int: found buffer: " << _rx_cur << endl;
                    buf = _rx_buffer[_rx_cur]; // Found a good one
                } else {
                    ++_rx_cur %= _rx_bufs;
                }
            }

            if (not buf) {
                db<TSTP_MAC>(WRN) << "TSTP_MAC::handle_int: no buffers left" << endl;
                db<TSTP_MAC>(WRN) << "TSTP_MAC::handle_int: dropping fifo contents" << endl;
                _phy->clear_rxfifo();
            } else {
                // We have a buffer, so we fetch a packet from the fifo
                auto sz = _phy->copy_from_rxfifo(reinterpret_cast<unsigned char *>(buf->frame()));
                buf->size(sz);

                auto * frame = buf->frame();

                db<TSTP_MAC>(TRC) << "TSTP_MAC::int:receive(d=" << frame->data<void>() << ",s=" << buf->size() << ")" << endl;

                db<TSTP_MAC>(INF) << "TSTP_MAC::handle_int[" << _rx_cur << "]" << endl;

                TSTP_MAC::update(buf);
            }
        }
    }
    db<TSTP_MAC>(TRC) << "TSTP_MAC::int: " << endl << "RFIRQF0 = " << hex << irqrf0 << endl;
    //if(irqrf0 & INT_RXMASKZERO) db<TSTP_MAC>(TRC) << "RXMASKZERO" << endl;
    //if(irqrf0 & INT_RXPKTDONE) db<TSTP_MAC>(TRC) << "RXPKTDONE" << endl;
    //if(irqrf0 & INT_FRAME_ACCEPTED) db<TSTP_MAC>(TRC) << "FRAME_ACCEPTED" << endl;
    //if(irqrf0 & INT_SRC_MATCH_FOUND) db<TSTP_MAC>(TRC) << "SRC_MATCH_FOUND" << endl;
    //if(irqrf0 & INT_SRC_MATCH_DONE) db<TSTP_MAC>(TRC) << "SRC_MATCH_DONE" << endl;
    //if(irqrf0 & INT_SFD) db<TSTP_MAC>(TRC) << "SFD" << endl;
    //if(irqrf0 & INT_ACT_UNUSED) db<TSTP_MAC>(TRC) << "ACT_UNUSED" << endl;

    db<TSTP_MAC>(TRC) << "RFIRQF1 = " << hex << irqrf1 << endl;
    //if(irqrf1 & INT_CSP_WAIT) db<TSTP_MAC>(TRC) << "CSP_WAIT" << endl;
    //if(irqrf1 & INT_CSP_STOP) db<TSTP_MAC>(TRC) << "CSP_STOP" << endl;
    //if(irqrf1 & INT_CSP_MANINT) db<TSTP_MAC>(TRC) << "CSP_MANINT" << endl;
    //if(irqrf1 & INT_RFIDLE) db<TSTP_MAC>(TRC) << "RFIDLE" << endl;
    //if(irqrf1 & INT_TXDONE) db<TSTP_MAC>(TRC) << "TXDONE" << endl;
    //if(irqrf1 & INT_TXACKDONE) db<TSTP_MAC>(TRC) << "TXACKDONE" << endl;
}

//template <>
//void TSTP_MAC::send_mf<CC2538_PHY>()
//{
//}
//
//template <>
//void TSTP_MAC::send_frame<CC2538_PHY>()
//{
//}

template<>
void TSTP_MAC::init<0, CC2538_PHY>(unsigned int unit)
{
    static const unsigned int TX_BUFS = Traits<TSTP>::MAC_Config<0>::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Traits<TSTP>::MAC_Config<0>::RECEIVE_BUFFERS;
    static const unsigned int DMA_BUFFER_SIZE = RX_BUFS * sizeof(Buffer) + TX_BUFS * sizeof(Buffer);
    IO_Irq irq = 26;
    DMA_Buffer * dma_buf = new (SYSTEM) DMA_Buffer(DMA_BUFFER_SIZE);
    CC2538_PHY * phy = new (SYSTEM) CC2538_PHY();
    _macs[unit].interrupt = IC::irq2int(irq);
    _macs[unit].mac = new (SYSTEM) TSTP_MAC(phy, TX_BUFS, RX_BUFS, dma_buf); // TODO: polymorphic PHY
}

template<>
void TSTP_MAC::init<1, CC2538_PHY>(unsigned int unit)
{
}

template<>
void TSTP_MAC::init<2, CC2538_PHY>(unsigned int unit)
{
}

__END_SYS
