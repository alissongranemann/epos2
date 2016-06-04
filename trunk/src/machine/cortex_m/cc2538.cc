// EPOS TI CC2538 IEEE 802.15.4 NIC Mediator Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <machine/cortex_m/machine.h>
#include <machine/cortex_m/cc2538.h>
#include <utility/malloc.h>

//#include <machine/cortex_m/emote3_gptm.h>
#include <utility/random.h>
//#include <gpio.h>

__BEGIN_SYS

// Class attributes
CC2538RF::Reg32 CC2538RF::Timer::_overflow_count;
CC2538RF::Reg32 CC2538RF::Timer::_interrupt_overflow_count;

CC2538::Device CC2538::_devices[UNITS];

// Methods
CC2538::~CC2538()
{
    db<CC2538>(TRC) << "~CC2538(unit=" << _unit << ")" << endl;
}

int CC2538::send(const Address & dst, const Type & type, const void * data, unsigned int size)
{
    if(size > Frame::MTU)
        size = Frame::MTU;

    db<CC2538>(TRC) << "Radio::send(s=" << address() << ",d=" << dst << ",p=" << hex << type << dec << ",d=" << data << ",s=" << size << ")" << endl;

    Buffer * buf = alloc(reinterpret_cast<NIC *>(this), dst, type, 0, 0, size);

    // Assemble the frame
    MAC::marshal(buf->frame(), address(), dst, type, data, size);

    return send(buf);
}


int CC2538::receive(Address * src, Type * type, void * data, unsigned int size)
{
    db<CC2538>(TRC) << "CC2538::receive(s=" << *src << ",p=" << hex << *type << dec << ",d=" << data << ",s=" << size << ") => " << endl;

    // Wait for a received frame and seize it
    for(bool locked = false; !locked; ) {
        locked = _rx_buffer[_rx_cur]->lock();
        if(!locked) ++_rx_cur;
    }

    // Wait for a complete frame to be received
    while(!_rx_done());

    int tmp;
    Buffer * buf = _rx_buffer[_rx_cur];

    if(frame_in_rxfifo()) {
        Phy_Frame * frame = buf->frame();

        // Move the frame from the NIC's RXFIFO into memory
        copy_from_nic(frame);

        // Disassemble the frame
        // Fixme: to MAC
//        *src = frame->src();
//        *type = frame->type();

        // For the upper layers, size will represent the size of frame->data<T>()
        buf->size(frame->length() - sizeof(Header) - sizeof(CRC) + sizeof(Phy_Header)); // Phy_Header is included in Header, but is already discounted in frame_length

        // Copy the data
        memcpy(data, frame->data<void>(), (buf->size() > size) ? size : buf->size());

        _statistics.rx_packets++;
        _statistics.rx_bytes += buf->size();

        db<CC2538>(INF) << "CC2538::receive done" << endl;

        tmp = buf->size();

        ++_rx_cur %= RX_BUFS;
    } else
        tmp = 0;

    buf->unlock();

    return tmp;
}


CC2538::Buffer * CC2538::alloc(NIC * nic, const Address & dst, const Type & type, unsigned int once, unsigned int always, unsigned int payload)
{
    db<CC2538>(TRC) << "CC2538::alloc(s=" << address() << ",d=" << dst << ",p=" << hex << type << dec << ",on=" << once << ",al=" << always << ",ld=" << payload << ")" << endl;

    // Initialize the buffer and assemble the Ethernet Frame Header
    return new (SYSTEM) Buffer(nic, once + always + payload);
}


int CC2538::send(Buffer * buf)
{
    db<CC2538>(TRC) << "CC2538::send(buf=" << buf << ")" << endl;
    db<CC2538>(INF) << "CC2538::send:frame=" << buf->frame() << " => " << *(buf->frame()) << endl;

    CC2538RF::copy_to_nic(buf->frame());
    unsigned int size = MAC::send(buf);

    if(size) {
        _statistics.tx_packets++;
        _statistics.tx_bytes += size;
    } else
        db<CC2538>(WRN) << "CC2538::send(buf=" << buf << ")" << " => failed!" << endl;

    delete buf;
    return size;
}


void CC2538::free(Buffer * buf)
{
    db<CC2538>(TRC) << "CC2538::free(buf=" << buf << ")" << endl;

    _statistics.rx_packets++;
    _statistics.rx_bytes += buf->size();

    buf->unlock();
}

void CC2538::reset()
{
    db<CC2538>(TRC) << "CC2538::reset()" << endl;

    // Reset statistics
    new (&_statistics) Statistics;
}


void CC2538::handle_int()
{
    Reg32 irqrf0 = sfr(RFIRQF0);
    Reg32 irqrf1 = sfr(RFIRQF1);
    Reg32 errf = sfr(RFERRF);
    sfr(RFIRQF0) = 0; 
    sfr(RFIRQF1) = 0;
    sfr(RFERRF) = 0;

    if(irqrf0 & INT_FIFOP) { // Frame received
        sfr(RFIRQF0) &= ~INT_FIFOP;
        if(frame_in_rxfifo()) {
            db<CC2538>(TRC) << "CC2538::handle_int: frame in rxfifo" << endl;
            Buffer * buf = 0;
            for(unsigned int i = 0; !buf && (i < RX_BUFS); ++i, ++_rx_cur %= RX_BUFS) {
                if(_rx_buffer[_rx_cur]->lock()) {
                    db<CC2538>(INF) << "CC2538::handle_int: found buffer: " << _rx_cur << endl;
                    buf = _rx_buffer[_rx_cur];
                }
            }
            if(!buf) {
                db<CC2538>(WRN) << "CC2538::handle_int: no buffers left! Dopping frame!" << endl;
                clear_rxfifo();
            } else {
                // We have a buffer, so we fetch a packet from the fifo
                Phy_Frame * frame = buf->frame();
                copy_from_nic(frame);
                buf->size(frame->length() - (sizeof(Header) + sizeof(CRC) - sizeof(Phy_Header))); // Phy_Header is included in Header, but is already discounted in frame_length

                db<CC2538>(TRC) << "CC2538::handle_int:receive(s=" /* << frame->src() << ",p=" << hex << frame->header()->type() << dec*/ << ",d=" << frame->data<void>() << ",s=" << buf->size() << ")" << endl;
                db<CC2538>(INF) << "CC2538::handle_int:rx_cur=" << _rx_cur << endl;

                // Fixme: MAC
                if(!notify(0, buf)) // No one was waiting for this frame, so let it free for receive()
                    free(buf);
            }
        }
    }

    db<CC2538>(TRC) << "CC2538::handle_int:RFIRQF0=" << hex << irqrf0 << endl;
    //if(irqrf0 & INT_RXMASKZERO) db<CC2538>(TRC) << "RXMASKZERO" << endl;
    //if(irqrf0 & INT_RXPKTDONE) db<CC2538>(TRC) << "RXPKTDONE" << endl;
    //if(irqrf0 & INT_FRAME_ACCEPTED) db<CC2538>(TRC) << "FRAME_ACCEPTED" << endl;
    //if(irqrf0 & INT_SRC_MATCH_FOUND) db<CC2538>(TRC) << "SRC_MATCH_FOUND" << endl;
    //if(irqrf0 & INT_SRC_MATCH_DONE) db<CC2538>(TRC) << "SRC_MATCH_DONE" << endl;
    //if(irqrf0 & INT_SFD) db<CC2538>(TRC) << "SFD" << endl;
    //if(irqrf0 & INT_ACT_UNUSED) db<CC2538>(TRC) << "ACT_UNUSED" << endl;

    db<CC2538>(TRC) << "CC2538::handle_int:RFIRQF1=" << hex << irqrf1 << endl;
    //if(irqrf1 & INT_CSP_WAIT) db<CC2538>(TRC) << "CSP_WAIT" << endl;
    //if(irqrf1 & INT_CSP_STOP) db<CC2538>(TRC) << "CSP_STOP" << endl;
    //if(irqrf1 & INT_CSP_MANINT) db<CC2538>(TRC) << "CSP_MANINT" << endl;
    //if(irqrf1 & INT_RFIDLE) db<CC2538>(TRC) << "RFIDLE" << endl;
    //if(irqrf1 & INT_TXDONE) db<CC2538>(TRC) << "TXDONE" << endl;
    //if(irqrf1 & INT_TXACKDONE) db<CC2538>(TRC) << "TXACKDONE" << endl;

    db<CC2538>(TRC) << "CC2538::handle_int:RFERRF=" << hex << errf << endl;
}


void CC2538::int_handler(const IC::Interrupt_Id & interrupt)
{
    CC2538 * dev = get_by_interrupt(interrupt);

    db<CC2538>(TRC) << "Radio::int_handler(int=" << interrupt << ",dev=" << dev << ")" << endl;

    if(!dev)
        db<CC2538>(WRN) << "Radio::int_handler: handler not assigned!" << endl;
    else
        dev->handle_int();
}

void CC2538RF::address(const NIC::Address & address)
{
    ffsm(SHORT_ADDR0) = address[0];
    ffsm(SHORT_ADDR1) = address[1];
}

//void CC2538RF::listen()
//{
//    // Clear interrupts
//    sfr(RFIRQF0) = 0;
//    sfr(RFIRQF1) = 0;
//    // Enable device interrupts
//    xreg(RFIRQM0) = INT_FIFOP;
//    xreg(RFIRQM1) = 0;
//    // Issue the listen command
//    rx();
//}
//
//void CC2538RF::stop_listening()
//{
//    // Disable device interrupts
//    xreg(RFIRQM0) = 0;
//    xreg(RFIRQM1) = 0;
//    // Issue the OFF command
//    off();
//}




// TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
// For now, we'll just copy using the RFDATA register
void CC2538RF::copy_from_nic(IEEE802_15_4::Phy_Frame * frame)
{
    char * buf = reinterpret_cast<char *>(frame);
    unsigned int len = sfr(RFDATA);  // First byte is the length of MAC frame
    buf[0] = len;
    for(unsigned int i = 1; i <= len; ++i)
        buf[i] = sfr(RFDATA);
    clear_rxfifo();
}


bool CC2538RF::frame_in_rxfifo()
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


__END_SYS

#endif
