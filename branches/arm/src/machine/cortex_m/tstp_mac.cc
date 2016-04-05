#include <tstp.h>
#include <ic.h>
#include <timer.h>

#include "../../../include/machine/cortex_m/cc2538_phy.h"

__BEGIN_SYS

template <>
void TSTP::MAC::send_frame<CC2538_PHY>(Buffer * buf) {
    db<TSTP::MAC>(TRC) << "TSTP::MAC::send_frame(buf=" << buf << ", sz=" << buf->size() << ")" << endl;

    User_Timer_3::delay(50000); // TODO: replace with proper MAC
    auto t = _tstp->time();
    buf->frame()->header()->last_hop_time(t);
    buf->frame()->header()->origin_time(t);
    auto f = reinterpret_cast<char *>(buf->frame());
    _phy->setup_tx(f, buf->size());
    _phy->tx();
    while(not _phy->tx_ok());
}

template <>
void TSTP::MAC::handle_int<CC2538_PHY>()
{
    Reg32 irqrf0 = _phy->sfr(_phy->RFIRQF0);
    Reg32 irqrf1 = _phy->sfr(_phy->RFIRQF1);

    if(irqrf0 & _phy->INT_SFD) { // Start of Frame
        _tstp->_time->frame_reception();
        _phy->sfr(_phy->RFIRQF0) &= ~_phy->INT_SFD;
    }

    if(irqrf0 & _phy->INT_FIFOP) { // Frame received
        _phy->sfr(_phy->RFIRQF0) &= ~_phy->INT_FIFOP;
        if(_phy->frame_in_rxfifo()) { // Checks CRC
            Buffer * buf = 0;

            // NIC received a frame in the RXFIFO, so we need to find an unused buffer for it
            for (auto i = 0u; (i < RX_BUFS) and not buf; ++i) {
                if (_rx_buffer[_rx_cur]->lock()) {
                    db<TSTP::MAC>(INF) << "TSTP::MAC::handle_int: found buffer: " << _rx_cur << endl;
                    buf = _rx_buffer[_rx_cur]; // Found a good one
                } else {
                    ++_rx_cur %= RX_BUFS;
                }
            }

            if (not buf) {
                db<TSTP::MAC>(WRN) << "TSTP::MAC::handle_int: no buffers left" << endl;
                db<TSTP::MAC>(WRN) << "TSTP::MAC::handle_int: dropping fifo contents" << endl;
                _phy->clear_rxfifo();
            } else {
                // We have a buffer, so we fetch a packet from the fifo
                auto sz = _phy->copy_from_rxfifo(reinterpret_cast<unsigned char *>(buf->frame()));
                buf->size(sz - sizeof(CRC));

                auto * frame = buf->frame();

                db<TSTP::MAC>(TRC) << "TSTP::MAC::int:receive(d=" << frame->as<void>() << ",s=" << buf->size() << ")" << endl;

                db<TSTP::MAC>(INF) << "TSTP::MAC::handle_int[" << _rx_cur << "]" << endl;

                TSTP::MAC::update(buf);
            }
        }
    }
    db<TSTP::MAC>(TRC) << "TSTP::MAC::int: " << endl << "RFIRQF0 = " << hex << irqrf0 << endl;
    //if(irqrf0 & INT_RXMASKZERO) db<TSTP::MAC>(TRC) << "RXMASKZERO" << endl;
    //if(irqrf0 & INT_RXPKTDONE) db<TSTP::MAC>(TRC) << "RXPKTDONE" << endl;
    //if(irqrf0 & INT_FRAME_ACCEPTED) db<TSTP::MAC>(TRC) << "FRAME_ACCEPTED" << endl;
    //if(irqrf0 & INT_SRC_MATCH_FOUND) db<TSTP::MAC>(TRC) << "SRC_MATCH_FOUND" << endl;
    //if(irqrf0 & INT_SRC_MATCH_DONE) db<TSTP::MAC>(TRC) << "SRC_MATCH_DONE" << endl;
    //if(irqrf0 & INT_SFD) db<TSTP::MAC>(TRC) << "SFD" << endl;
    //if(irqrf0 & INT_ACT_UNUSED) db<TSTP::MAC>(TRC) << "ACT_UNUSED" << endl;

    db<TSTP::MAC>(TRC) << "RFIRQF1 = " << hex << irqrf1 << endl;
    //if(irqrf1 & INT_CSP_WAIT) db<TSTP::MAC>(TRC) << "CSP_WAIT" << endl;
    //if(irqrf1 & INT_CSP_STOP) db<TSTP::MAC>(TRC) << "CSP_STOP" << endl;
    //if(irqrf1 & INT_CSP_MANINT) db<TSTP::MAC>(TRC) << "CSP_MANINT" << endl;
    //if(irqrf1 & INT_RFIDLE) db<TSTP::MAC>(TRC) << "RFIDLE" << endl;
    //if(irqrf1 & INT_TXDONE) db<TSTP::MAC>(TRC) << "TXDONE" << endl;
    //if(irqrf1 & INT_TXACKDONE) db<TSTP::MAC>(TRC) << "TXACKDONE" << endl;
}

__END_SYS
