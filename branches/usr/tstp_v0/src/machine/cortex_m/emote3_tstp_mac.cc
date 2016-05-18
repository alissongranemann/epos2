//// EPOS eMote3_TSTP_MAC IEEE 802.15.4 NIC Mediator Implementation
//
//#include <system/config.h>
//#ifndef __no_networking__
//
//#include <machine/cortex_m/machine.h>
//#include <machine/cortex_m/emote3_gptm.h>
//#include "../../../include/machine/cortex_m/emote3_tstp_mac.h"
//#include <utility/malloc.h>
//#include <utility/random.h>
//#include <alarm.h>
//#include <gpio.h>
//
//__BEGIN_SYS
//
//// Class attributes
//eMote3_TSTP_MAC::Device eMote3_TSTP_MAC::_devices[UNITS];
//
//
//// Methods
//eMote3_TSTP_MAC::~eMote3_TSTP_MAC() { db<eMote3_TSTP_MAC>(TRC) << "~Radio(unit=" << _unit << ")" << endl; }
//
//int eMote3_TSTP_MAC::send(const Address & dst, const Protocol & prot, const void * data, unsigned int size)
//{
//    if(size > MTU) {
//        return 0;
//    }
//    if(auto b = alloc(NIC::get(_unit), dst, prot, 0, 0, size)) {
//        memcpy(b->frame(), data, size);
//        return send(b);
//    } else {
//        return 0;
//    }
//}
//
//eMote3_TSTP_MAC::Buffer * eMote3_TSTP_MAC::alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload)
//{
//    db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::alloc(s=" << _address << ",d=" << dst << ",p=" << hex << prot << dec << ",on=" << once << ",al=" << always << ",ld=" << payload << ")" << endl;
//
//    // TSTP_MAC does not support fragmentation
//    if(once + always + payload > MTU)
//        return 0;
//
//    // Wait for the next buffer to become free and seize it
//    for(bool locked = false; !locked; ) {
//        locked = _tx_buffer[_tx_cur]->lock();
//        if(!locked) ++_tx_cur %= TX_BUFS;
//    }
//    Buffer * buf = _tx_buffer[_tx_cur];
//
//    // Initialize the buffer
//    auto sz = once + always + payload;
//    new (buf) Buffer(nic, sz, _address, dst, prot, sz);
//
//    db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::alloc[" << _tx_cur << "]" << endl;
//
//    ++_tx_cur %= TX_BUFS;
//
//    return buf;
//}
//
//int eMote3_TSTP_MAC::send(Buffer * buf)
//{
//    db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::send(buf=" << buf << ")" << endl;
//
//    buf->set_id();
//    _tx_ready_schedule.insert(buf->tx_link());
//    /*
//    char * f = reinterpret_cast<char *>(buf->frame());
//
//    clear_txfifo();
//    setup_tx(f, buf->size());
//
//    // Trigger an immediate send poll
//    bool ok = send_and_wait(ack);
//
//    if(ok) {
//        db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::send done" << endl;
//        _statistics.tx_packets++;
//        _statistics.tx_bytes += buf->size();
//        size += buf->size();
//    } else {
//        db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::send failed!" << endl;
//    }
//    */
//    return buf->size();
//}
//
//
//void eMote3_TSTP_MAC::free(Buffer * buf)
//{
//    db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::free(buf=" << buf << ")" << endl;
//
//    for(Buffer::Element * el = buf->link(); el; el = el->next()) {
//        buf = el->object();
//
//        _statistics.rx_packets++;
//        _statistics.rx_bytes += buf->size();
//
//        // Release the buffer to the OS
//        buf->unlock();
//
//        db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::free " << buf << endl;
//    }
//}
//
//void eMote3_TSTP_MAC::reset()
//{
//    db<eMote3_TSTP_MAC>(TRC) << "Radio::reset()" << endl;
//
//    // Reset statistics
//    new (&_statistics) Statistics;
//}
//
//void eMote3_TSTP_MAC::handle_int()
//{
//    Reg32 irqrf0 = sfr(RFIRQF0);
//    Reg32 irqrf1 = sfr(RFIRQF1);
//
//    if(irqrf0 & INT_FIFOP) { // Frame received
//        sfr(RFIRQF0) &= ~INT_FIFOP;
//        if(frame_in_rxfifo()) {
//            Buffer * buf = 0;
//
//            // NIC received a frame in the RXFIFO, so we need to find an unused buffer for it
//            for (auto i = 0u; (i < RX_BUFS) and not buf; ++i) {
//                if (_rx_buffer[_rx_cur]->lock()) {
//                    db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::handle_int: found buffer: " << _rx_cur << endl;
//                    buf = _rx_buffer[_rx_cur]; // Found a good one
//                } else {
//                    ++_rx_cur %= RX_BUFS;
//                }
//            }
//
//            if (not buf) {
//                db<eMote3_TSTP_MAC>(WRN) << "eMote3_TSTP_MAC::handle_int: no buffers left" << endl;
//                db<eMote3_TSTP_MAC>(WRN) << "eMote3_TSTP_MAC::handle_int: dropping fifo contents" << endl;
//                clear_rxfifo();
//            } else {
//                // We have a buffer, so we fetch a packet from the fifo
//                auto b = reinterpret_cast<unsigned char *>(buf->frame());
//                auto sz = copy_from_rxfifo(b + 1);
//                b[0] = sz;
//                buf->size(buf->frame()->frame_length() - (sizeof(Header) + sizeof(CRC) - sizeof(Phy_Header))); // Phy_Header is included in Header, but is already discounted in frame_length
//
//                auto * frame = buf->frame();
//
//                db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::int:receive(s=" << frame->src() << ",p=" << hex << frame->header()->prot() << dec
//                    << ",d=" << frame->data<void>() << ",s=" << buf->size() << ")" << endl;
//
//                db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::handle_int[" << _rx_cur << "]" << endl;
//
//                //IC::disable(_irq);
//                if(!notify(frame->header()->prot(), buf)) {// No one was waiting for this frame, so let it free for receive()
//                    free(buf);
//                }
//                // TODO: this serialization is much too restrictive. It was done this way for students to play with
//                //IC::enable(_irq);
//            }
//        }
//    }
//    db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::int: " << endl << "RFIRQF0 = " << hex << irqrf0 << endl;
//    //if(irqrf0 & INT_RXMASKZERO) db<eMote3_TSTP_MAC>(TRC) << "RXMASKZERO" << endl;
//    //if(irqrf0 & INT_RXPKTDONE) db<eMote3_TSTP_MAC>(TRC) << "RXPKTDONE" << endl;
//    //if(irqrf0 & INT_FRAME_ACCEPTED) db<eMote3_TSTP_MAC>(TRC) << "FRAME_ACCEPTED" << endl;
//    //if(irqrf0 & INT_SRC_MATCH_FOUND) db<eMote3_TSTP_MAC>(TRC) << "SRC_MATCH_FOUND" << endl;
//    //if(irqrf0 & INT_SRC_MATCH_DONE) db<eMote3_TSTP_MAC>(TRC) << "SRC_MATCH_DONE" << endl;
//    //if(irqrf0 & INT_SFD) db<eMote3_TSTP_MAC>(TRC) << "SFD" << endl;
//    //if(irqrf0 & INT_ACT_UNUSED) db<eMote3_TSTP_MAC>(TRC) << "ACT_UNUSED" << endl;
//
//    db<eMote3_TSTP_MAC>(TRC) << "RFIRQF1 = " << hex << irqrf1 << endl;
//    //if(irqrf1 & INT_CSP_WAIT) db<eMote3_TSTP_MAC>(TRC) << "CSP_WAIT" << endl;
//    //if(irqrf1 & INT_CSP_STOP) db<eMote3_TSTP_MAC>(TRC) << "CSP_STOP" << endl;
//    //if(irqrf1 & INT_CSP_MANINT) db<eMote3_TSTP_MAC>(TRC) << "CSP_MANINT" << endl;
//    //if(irqrf1 & INT_RFIDLE) db<eMote3_TSTP_MAC>(TRC) << "RFIDLE" << endl;
//    //if(irqrf1 & INT_TXDONE) db<eMote3_TSTP_MAC>(TRC) << "TXDONE" << endl;
//    //if(irqrf1 & INT_TXACKDONE) db<eMote3_TSTP_MAC>(TRC) << "TXACKDONE" << endl;
//}
//
//
//void eMote3_TSTP_MAC::int_handler(const IC::Interrupt_Id & interrupt)
//{
//    eMote3_TSTP_MAC * dev = get_by_interrupt(interrupt);
//
//    db<eMote3_TSTP_MAC>(TRC) << "Radio::int_handler(int=" << interrupt << ",dev=" << dev << ")" << endl;
//
//    if(!dev)
//        db<eMote3_TSTP_MAC>(WRN) << "Radio::int_handler: handler not assigned!" << endl;
//    else
//        dev->handle_int();
//}
//
//// State machine
//void eMote3_TSTP_MAC::check_tx_schedule()
//{
//    db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::check_tx_schedule()" << endl;
//    //_rx_pin.clear();
//    //_tx_pin.clear();
//    off();
//    auto t = time();
//    bool new_ready_frame = false;
//    for(auto head = _retransmission_schedule.head(); head and (head->rank() <= t); head = _tx_later_schedule.head()) {
//        _retransmission_schedule.remove_head();
//        auto deadline = head->object()->deadline();
//        if((deadline > t) and ((deadline - t) > Config::PERIOD)) {
//            new_ready_frame = true;
//            head->rank(head->object()->deadline());
//            _tx_ready_schedule.insert(head);
//        } else {
//            free(head->object());
//        }
//    }
//    // Channel was silent in the last two rounds, this means that no one is trying to forward my message. Retransmit now.
//    if(not new_ready_frame and (_channel_silent >= 2) and _retransmission_schedule.head()) {
//        auto head = _retransmission_schedule.remove_head();
//        head->rank(head->object()->frame()->deadline());
//        _tx_ready_schedule.insert(head);
//    }
//    auto head = _tx_ready_schedule.head();
//    for(; head and (head->rank() != 0) and ((head->rank() <= t) or ((head->rank() - t) <= Config::PERIOD)); _tx_ready_schedule.remove_head(), free(head->object()), head = _tx_ready_schedule.head());
//    _tx_pending = head;
//    if(head) {
//        Time backoff;
//        if(head->rank() == 0) {
//            auto id = head->object()->id();
//            if(id != _last_fwd_id) {
//                _last_backoff = tstp()->_router->backoff(reinterpret_cast<char *>(head->object()->frame()));
//                _last_fwd_id = id;
//            }
//            backoff = _last_backoff;
//        } else {
//            backoff = tstp()->_router->backoff(reinterpret_cast<char *>(head->object()->frame()));
//        }
//        _cca.schedule(t + backoff);
//    } else {
//        _rx_mf.schedule(t + Config::SLEEP_PERIOD);
//    }
//}
//
//__END_SYS
//
//#endif
