// EPOS eMote3_TSTP_MAC IEEE 802.15.4 NIC Mediator Implementation
/*

#include <system/config.h>
#ifndef __no_networking__

#include <machine/cortex_m/machine.h>
#include <machine/cortex_m/emote3_gptm.h>
#include "../../../include/machine/cortex_m/emote3_tstp_mac.h"
#include <utility/malloc.h>
#include <utility/random.h>
#include <alarm.h>
#include <gpio.h>

__BEGIN_SYS

// Class attributes
eMote3_TSTP_MAC::Device eMote3_TSTP_MAC::_devices[UNITS];

IC::Interrupt_Handler MAC_Timer::_user_handler;
CPU::Reg32 MAC_Timer::_overflow_count_overflow;
CPU::Reg32 MAC_Timer::_int_overflow_count_overflow;

// Methods
eMote3_TSTP_MAC::~eMote3_TSTP_MAC()
{
    db<eMote3_TSTP_MAC>(TRC) << "~Radio(unit=" << _unit << ")" << endl;
}

int eMote3_TSTP_MAC::send(const Address & dst, const Protocol & prot, const void * data, unsigned int size)
{
    if(size > MTU) {
        return 0;
    }
    if(auto b = alloc(reinterpret_cast<NIC*>(this), dst, prot, 0, 0, size)) {
        b->frame()->data(data);
        return send(b);
    } else {
        return 0;
    }
}

int eMote3_TSTP_MAC::receive(Address * src, Protocol * prot, void * data, unsigned int size)
{
    db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::receive(s=" << *src << ",p=" << hex << *prot << dec
        << ",d=" << data << ",s=" << size << ") => " << endl;

    // Wait for a received frame and seize it
    for(bool locked = false; !locked; ) {
        locked = _rx_buffer[_rx_cur]->lock();
        if(!locked) ++_rx_cur;
    }

    // Wait for a complete frame to be received
    while(!_rx_done());

    Buffer * buf = _rx_buffer[_rx_cur];

    if(frame_in_rxfifo()) {
        auto b = reinterpret_cast<unsigned char *>(buf->frame());
        auto sz = copy_from_rxfifo(b + 1);
        b[0] = sz;

        // Disassemble the frame
        Frame * frame = buf->frame();
        *src = frame->src();
        *prot = frame->prot();

        // For the upper layers, size will represent the size of frame->data<T>()
        buf->size(buf->frame()->frame_length() - sizeof(Header) - sizeof(CRC) + sizeof(Phy_Header)); // Phy_Header is included in Header, but is already discounted in frame_length

        // Copy the data
        memcpy(data, frame->data<void>(), (buf->size() > size) ? size : buf->size());

        _statistics.rx_packets++;
        _statistics.rx_bytes += buf->size();

        db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::receive done" << endl;

        int tmp = buf->size();

        buf->unlock();

        ++_rx_cur %= RX_BUFS;

        return tmp;
    } else {
        buf->unlock();
        return 0;
    }
}

eMote3_TSTP_MAC::Buffer * eMote3_TSTP_MAC::alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload)
{
    db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::alloc(s=" << _address << ",d=" << dst << ",p=" << hex << prot << dec << ",on=" << once << ",al=" << always << ",ld=" << payload << ")" << endl;

    int max_data = MTU - always;

    // TODO: replace with division
    unsigned int buffers = 0;
    for(int size = once + payload; size > 0; size -= max_data, buffers++);
    if(buffers > TX_BUFS) {
//    if((payload + once) / max_data > TX_BUFS) {
        db<eMote3_TSTP_MAC>(WRN) << "eMote3_TSTP_MAC::alloc: sizeof(Network::Packet::Data) > sizeof(NIC::Frame::Data) * TX_BUFS!" << endl;
        return 0;
    }

    Buffer::List pool;

    // Calculate how many frames are needed to hold the transport PDU and allocate enough buffers
    for(int size = once + payload; size > 0; size -= max_data) {
        // Wait for the next buffer to become free and seize it
        for(bool locked = false; !locked; ) {
            locked = _tx_buffer[_tx_cur]->lock();
            if(!locked) ++_tx_cur %= TX_BUFS;
        }
        Buffer * buf = _tx_buffer[_tx_cur];

        // Initialize the buffer
        auto sz = (size > max_data) ? MTU : size + always;
        new (buf) Buffer(nic, sz, _address, dst, prot, sz);

        db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::alloc[" << _tx_cur << "]" << endl;

        ++_tx_cur %= TX_BUFS;

        pool.insert(buf->link());
    }

    return pool.head()->object();
}

int eMote3_TSTP_MAC::send(Buffer * buf)
{
    int size = 0;

    for(Buffer::Element * el = buf->link(); el; el = el->next()) {
        buf = el->object();
        const bool ack = Traits<eMote3_TSTP_MAC>::ACK and (buf->frame()->dst() != broadcast());

        db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::send(buf=" << buf << ")" << endl;

        // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
        // For now, we'll just copy using the RFDATA register
        char * f = reinterpret_cast<char *>(buf->frame());

        sfr(RFST) = ISFLUSHTX; // Clear TXFIFO
        while(xreg(TXFIFOCNT) != 0);

        // First field is length of MAC
        // CRC is inserted by hardware (assuming auto-CRC is enabled)
        const int crc_size = sizeof(CRC);
        for(int i=0; i < f[0] + 1 - crc_size; i++)
            sfr(RFDATA) = f[i];

        // Trigger an immediate send poll
        bool ok = send_and_wait(ack);

        if(ok) {
            db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::send done" << endl;
            _statistics.tx_packets++;
            _statistics.tx_bytes += buf->size();
            size += buf->size();
        } else {
            db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::send failed!" << endl;
        }

        buf->unlock();
    }

    return size;
}


void eMote3_TSTP_MAC::free(Buffer * buf)
{
    db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::free(buf=" << buf << ")" << endl;

    for(Buffer::Element * el = buf->link(); el; el = el->next()) {
        buf = el->object();

        _statistics.rx_packets++;
        _statistics.rx_bytes += buf->size();

        // Release the buffer to the OS
        buf->unlock();

        db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::free " << buf << endl;
    }
}


void eMote3_TSTP_MAC::reset()
{
    db<eMote3_TSTP_MAC>(TRC) << "Radio::reset()" << endl;

    // Reset statistics
    new (&_statistics) Statistics;
}

// TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
// For now, we'll just copy using the RFDATA register
unsigned int CC2538_PHY::copy_from_rxfifo(unsigned char * data)
{
    unsigned int sz = sfr(RFDATA); // First field is length of MAC frame
    for(auto i = 0u; i < sz; ++i) { // Copy MAC frame
        data[i] = sfr(RFDATA);
    }
    clear_rxfifo();
    return sz;
}

bool eMote3_TSTP_MAC::wait_for_ack()
{
    while(!(sfr(RFIRQF1) & INT_TXDONE));
    sfr(RFIRQF1) &= ~INT_TXDONE;

    if(not Traits<eMote3_TSTP_MAC>::auto_listen) {
        xreg(RFST) = ISRXON;
    }

    bool acked = false;
    eMote3_GPTM timer(2, Traits<eMote3_TSTP_MAC>::ACK_TIMEOUT);
    timer.enable();
    while(timer.running() and not (acked = (sfr(RFIRQF0) & INT_FIFOP)));

    return acked;
}

bool eMote3_TSTP_MAC::send_and_wait(bool ack)
{
    bool do_ack = Traits<eMote3_TSTP_MAC>::ACK and ack;
    Reg32 saved_filter_settings = 0;
    if(do_ack) {
        saved_filter_settings = xreg(FRMFILT1);
        xreg(RFIRQM0) &= ~INT_FIFOP; // Disable FIFOP int. We'll poll the interrupt flag
        xreg(FRMFILT1) = ACCEPT_FT2_ACK; // Accept only ACK frames now
    }

    bool sent = backoff_and_send();

    if(do_ack) {
        bool acked = sent and wait_for_ack();

        for(auto i = 0u; (i < Traits<eMote3_TSTP_MAC>::RETRANSMISSIONS) and not acked; i++) {
            db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::retransmitting" << endl;
            sent = backoff_and_send();

            acked = sent and wait_for_ack();
        }

        if(acked) {
            sfr(RFIRQF0) &= ~INT_FIFOP; // Clear FIFOP flag
            clear_rxfifo();
        }

        if(not Traits<eMote3_TSTP_MAC>::auto_listen) {
            xreg(RFST) = ISRFOFF;
        }

        xreg(FRMFILT1) = saved_filter_settings; // Done with ACKs
        xreg(RFIRQM0) |= INT_FIFOP; // Enable FIFOP int
        return acked;
    }
    else if(sent) {
        while(!(sfr(RFIRQF1) & INT_TXDONE));
        sfr(RFIRQF1) &= ~INT_TXDONE;
    }

    return sent;
}

bool eMote3_TSTP_MAC::backoff_and_send()
{
    bool ret = true;
    if(Traits<eMote3_TSTP_MAC>::CSMA_CA) {
        start_cca();

        unsigned int two_raised_to_be = 1;
        unsigned int BE;
        for(BE = 0; BE < Traits<eMote3_TSTP_MAC>::CSMA_CA_MIN_BACKOFF_EXPONENT; BE++) {
            two_raised_to_be *= 2;
        }

        unsigned int trials;
        for(trials = 0u; trials < Traits<eMote3_TSTP_MAC>::CSMA_CA_MAX_TRANSMISSION_TRIALS; trials++) {
            const auto ubp = Traits<eMote3_TSTP_MAC>::CSMA_CA_UNIT_BACKOFF_PERIOD;
            auto delay_time = (Random::random() % (two_raised_to_be - 1)) * ubp;
            delay_time = delay_time < ubp ? ubp : delay_time;

            eMote3_GPTM::delay(delay_time, 2);
            if(tx_if_cca()) {
                break; // Success
            }
            
            if(BE < Traits<eMote3_TSTP_MAC>::CSMA_CA_MAX_BACKOFF_EXPONENT) {
                BE++;
                two_raised_to_be *= 2;
            }
        }

        end_cca();

        if(trials >= Traits<eMote3_TSTP_MAC>::CSMA_CA_MAX_TRANSMISSION_TRIALS) {
            db<eMote3_TSTP_MAC>(WRN) << "eMote3_TSTP_MAC::backoff_and_send() FAILED" << endl;
            ret = false;
        }
    }
    else {
        tx();
    }

    return ret;
}

bool CC2538_PHY::frame_in_rxfifo()
{
    bool ret = false;
    if(xreg(RXFIFOCNT) > 0) {
        auto rxfifo = reinterpret_cast<volatile unsigned int*>(RXFIFO);
        unsigned char mac_frame_size = rxfifo[0];
        if (mac_frame_size > 127) {
            clear_rxfifo();
            ret = false;
        }
        else {
            // On RX, last byte in the frame contains info like CRC result
            // (obs: mac frame is preceeded by one byte containing the frame length, 
            // so total rxfifo data's size is 1 + mac_frame_size)
            ret = rxfifo[mac_frame_size] & AUTO_CRC_OK;
            
            if(not ret) {
                clear_rxfifo();
            }
        }
    }

    return ret;
}

void eMote3_TSTP_MAC::handle_int()
{
    Reg32 irqrf0 = sfr(RFIRQF0);
    Reg32 irqrf1 = sfr(RFIRQF1);

    if(irqrf0 & INT_FIFOP) { // Frame received
        sfr(RFIRQF0) &= ~INT_FIFOP;
        if(frame_in_rxfifo()) {
            Buffer * buf = 0;

            // NIC received a frame in the RXFIFO, so we need to find an unused buffer for it
            for (auto i = 0u; (i < RX_BUFS) and not buf; ++i) {
                if (_rx_buffer[_rx_cur]->lock()) {
                    db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::handle_int: found buffer: " << _rx_cur << endl;
                    buf = _rx_buffer[_rx_cur]; // Found a good one
                } else {
                    ++_rx_cur %= RX_BUFS;
                }
            }

            if (not buf) {
                db<eMote3_TSTP_MAC>(WRN) << "eMote3_TSTP_MAC::handle_int: no buffers left" << endl;
                db<eMote3_TSTP_MAC>(WRN) << "eMote3_TSTP_MAC::handle_int: dropping fifo contents" << endl;
                clear_rxfifo();
            } else {
                // We have a buffer, so we fetch a packet from the fifo
                auto b = reinterpret_cast<unsigned char *>(buf->frame());
                auto sz = copy_from_rxfifo(b + 1);
                b[0] = sz;
                buf->size(buf->frame()->frame_length() - (sizeof(Header) + sizeof(CRC) - sizeof(Phy_Header))); // Phy_Header is included in Header, but is already discounted in frame_length

                auto * frame = buf->frame();

                db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::int:receive(s=" << frame->src() << ",p=" << hex << frame->header()->prot() << dec
                    << ",d=" << frame->data<void>() << ",s=" << buf->size() << ")" << endl;

                db<eMote3_TSTP_MAC>(INF) << "eMote3_TSTP_MAC::handle_int[" << _rx_cur << "]" << endl;

                //IC::disable(_irq);
                if(!notify(frame->header()->prot(), buf)) {// No one was waiting for this frame, so let it free for receive()
                    free(buf);
                }
                // TODO: this serialization is much too restrictive. It was done this way for students to play with
                //IC::enable(_irq);
            }
        }
    }
    db<eMote3_TSTP_MAC>(TRC) << "eMote3_TSTP_MAC::int: " << endl << "RFIRQF0 = " << hex << irqrf0 << endl;
    //if(irqrf0 & INT_RXMASKZERO) db<eMote3_TSTP_MAC>(TRC) << "RXMASKZERO" << endl;
    //if(irqrf0 & INT_RXPKTDONE) db<eMote3_TSTP_MAC>(TRC) << "RXPKTDONE" << endl;
    //if(irqrf0 & INT_FRAME_ACCEPTED) db<eMote3_TSTP_MAC>(TRC) << "FRAME_ACCEPTED" << endl;
    //if(irqrf0 & INT_SRC_MATCH_FOUND) db<eMote3_TSTP_MAC>(TRC) << "SRC_MATCH_FOUND" << endl;
    //if(irqrf0 & INT_SRC_MATCH_DONE) db<eMote3_TSTP_MAC>(TRC) << "SRC_MATCH_DONE" << endl;
    //if(irqrf0 & INT_SFD) db<eMote3_TSTP_MAC>(TRC) << "SFD" << endl;
    //if(irqrf0 & INT_ACT_UNUSED) db<eMote3_TSTP_MAC>(TRC) << "ACT_UNUSED" << endl;

    db<eMote3_TSTP_MAC>(TRC) << "RFIRQF1 = " << hex << irqrf1 << endl;
    //if(irqrf1 & INT_CSP_WAIT) db<eMote3_TSTP_MAC>(TRC) << "CSP_WAIT" << endl;
    //if(irqrf1 & INT_CSP_STOP) db<eMote3_TSTP_MAC>(TRC) << "CSP_STOP" << endl;
    //if(irqrf1 & INT_CSP_MANINT) db<eMote3_TSTP_MAC>(TRC) << "CSP_MANINT" << endl;
    //if(irqrf1 & INT_RFIDLE) db<eMote3_TSTP_MAC>(TRC) << "RFIDLE" << endl;
    //if(irqrf1 & INT_TXDONE) db<eMote3_TSTP_MAC>(TRC) << "TXDONE" << endl;
    //if(irqrf1 & INT_TXACKDONE) db<eMote3_TSTP_MAC>(TRC) << "TXACKDONE" << endl;
}


void eMote3_TSTP_MAC::int_handler(const IC::Interrupt_Id & interrupt)
{
    eMote3_TSTP_MAC * dev = get_by_interrupt(interrupt);

    db<eMote3_TSTP_MAC>(TRC) << "Radio::int_handler(int=" << interrupt << ",dev=" << dev << ")" << endl;

    if(!dev)
        db<eMote3_TSTP_MAC>(WRN) << "Radio::int_handler: handler not assigned!" << endl;
    else
        dev->handle_int();
}

__END_SYS

#endif
*/
