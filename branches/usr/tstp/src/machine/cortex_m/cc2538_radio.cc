// EPOS CC2538 IEEE 802.15.4 NIC Mediator Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <machine/cortex_m/machine.h>
#include "../../../include/machine/cortex_m/cc2538_radio.h"
#include <utility/malloc.h>
#include <alarm.h>

__BEGIN_SYS

// Class attributes
CC2538::Device CC2538::_devices[UNITS];

// Methods
CC2538::~CC2538()
{
    db<CC2538>(TRC) << "~Radio(unit=" << _unit << ")" << endl;
}


int CC2538::send(const Address & dst, const Protocol & prot, const void * data, unsigned int size)
{
    // Wait for a buffer to become free and seize it
    for(bool locked = false; !locked; ) {
        for(; _tx_ring[_tx_cur].status & Tx_Desc::OWN; ++_tx_cur %= TX_BUFS);
        locked = _tx_buffer[_tx_cur]->lock();
    }

    Tx_Desc * desc = &_tx_ring[_tx_cur];
    Buffer * buf = _tx_buffer[_tx_cur];

    db<CC2538>(TRC) << "Radio::send(s=" << _address << ",d=" << dst << ",p=" << hex << prot << dec
                     << ",d=" << data << ",s=" << size << ")" << endl;

    // Assemble the Ethernet frame
    new (buf->frame()) Frame(_address, dst, prot, data, size);

    desc->size = -(size + sizeof(Header)); // 2's comp.

    // Status must be set last, since it can trigger a send
    desc->status = Tx_Desc::OWN | Tx_Desc::STP | Tx_Desc::ENP;

    // Trigger an immediate send poll
//    csr(0, csr(0) | CSR0_TDMD);

    _statistics.tx_packets++;
    _statistics.tx_bytes += size;

    // Wait for packet to be sent
    // while(desc->status & Tx_Desc::OWN);

    db<CC2538>(INF) << "Radio::send:desc[" << _tx_cur << "]=" << desc << " => " << *desc << endl;

    buf->unlock();

    ++_tx_cur %= TX_BUFS;

    return size;
}


int CC2538::receive(Address * src, Protocol * prot, void * data, unsigned int size)
{
    db<CC2538>(TRC) << "Radio::receive(s=" << *src << ",p=" << hex << *prot << dec
                     << ",d=" << data << ",s=" << size << ") => " << endl;

    // Wait for a received frame and seize it
    for(bool locked = false; !locked; ) {
        for(; _rx_ring[_rx_cur].status & Rx_Desc::OWN; ++_rx_cur %= RX_BUFS);
        locked = _rx_buffer[_rx_cur]->lock();
    }
    Buffer * buf = _rx_buffer[_rx_cur];
    Rx_Desc * desc = &_rx_ring[_rx_cur];

    // Disassemble the Ethernet frame
    Frame * frame = buf->frame();
    *src = frame->src();
    *prot = frame->prot();

    // For the upper layers, size will represent the size of frame->data<T>()
    buf->size((desc->misc & 0x00000fff) - sizeof(Header) - sizeof(CRC));

    // Copy the data
    memcpy(data, frame->data<void>(), (buf->size() > size) ? size : buf->size());

    // Release the buffer to the NIC
    desc->status = Rx_Desc::OWN;

    _statistics.rx_packets++;
    _statistics.rx_bytes += buf->size();

    db<CC2538>(INF) << "Radio::receive:desc[" << _rx_cur << "]=" << desc << " => " << *desc << endl;

    int tmp = buf->size();

    buf->unlock();

    ++_rx_cur %= RX_BUFS;

    return tmp;
}


// Allocated buffers must be sent or release IN ORDER as assumed by the Radio
CC2538::Buffer * CC2538::alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload)
{
    db<CC2538>(TRC) << "Radio::alloc(s=" << _address << ",d=" << dst << ",p=" << hex << prot << dec << ",on=" << once << ",al=" << always << ",ld=" << payload << ")" << endl;

    int max_data = MTU - always;

    if((payload + once) / max_data > TX_BUFS) {
        db<CC2538>(WRN) << "Radio::alloc: sizeof(Network::Packet::Data) > sizeof(NIC::Frame::Data) * TX_BUFS!" << endl;
        return 0;
    }

    Buffer::List pool;

    // Calculate how many frames are needed to hold the transport PDU and allocate enough buffers
    for(int size = once + payload; size > 0; size -= max_data) {
        // Wait for the next buffer to become free and seize it
        for(bool locked = false; !locked; ) {
            for(; _tx_ring[_tx_cur].status & Tx_Desc::OWN; ++_tx_cur %= TX_BUFS);
            locked = _tx_buffer[_tx_cur]->lock();
        }
        Tx_Desc * desc = &_tx_ring[_tx_cur];
        Buffer * buf = _tx_buffer[_tx_cur];

        // Initialize the buffer and assemble the Ethernet Frame Header
        new (buf) Buffer(nic, _address, dst, prot, (size > max_data) ? MTU : size + always);

        db<CC2538>(INF) << "Radio::alloc:desc[" << _tx_cur << "]=" << desc << " => " << *desc << endl;

        ++_tx_cur %= TX_BUFS;

        pool.insert(buf->link());
    }

    return pool.head()->object();
}


int CC2538::send(Buffer * buf)
{
    unsigned int size = 0;

    for(Buffer::Element * el = buf->link(); el; el = el->next()) {
        buf = el->object();
        Tx_Desc * desc = buf->back<Tx_Desc>();

        db<CC2538>(TRC) << "Radio::send(buf=" << buf << ")" << endl;

        desc->size = -(buf->size() + sizeof(Header)); // 2's comp.

        // Status must be set last, since it can trigger a send
        desc->status = Tx_Desc::OWN | Tx_Desc::STP | Tx_Desc::ENP;

        // Trigger an immediate send poll
//        csr(0, csr(0) | CSR0_TDMD);

        size += buf->size();

        _statistics.tx_packets++;
        _statistics.tx_bytes += buf->size();

        db<CC2538>(INF) << "Radio::send:desc=" << desc << " => " << *desc << endl;

        // Wait for packet to be sent and unlock the respective buffer
        while(desc->status & Tx_Desc::OWN);
        buf->unlock();
    }

    return size;
}


void CC2538::free(Buffer * buf)
{
    db<CC2538>(TRC) << "Radio::free(buf=" << buf << ")" << endl;

    for(Buffer::Element * el = buf->link(); el; el = el->next()) {
        buf = el->object();
        Rx_Desc * desc = buf->back<Rx_Desc>();

        _statistics.rx_packets++;
        _statistics.rx_bytes += buf->size();

        // Release the buffer to the NIC
        desc->size = Reg16(-sizeof(Frame)); // 2's comp.
        desc->status = Rx_Desc::OWN; // Owned by NIC

        // Release the buffer to the OS
        buf->unlock();

        db<CC2538>(INF) << "Radio::free:desc=" << desc << " => " << *desc << endl;
    }
}


void CC2538::reset()
{
    db<CC2538>(TRC) << "Radio::reset()" << endl;


    // Reset statistics
    new (&_statistics) Statistics;
}


void CC2538::handle_int()
{
        if(true) { // Frame received (possibly multiple, let's handle a whole round on the ring buffer)

            // Note that ISRs in EPOS are reentrant, that's why locking was carefully made atomic
            // Therefore, several instances of this code can compete to handle received buffers

            for(int count = RX_BUFS; count && !(_rx_ring[_rx_cur].status & Rx_Desc::OWN); count--, ++_rx_cur %= RX_BUFS) {
                // NIC received a frame in _rx_buffer[_rx_cur], let's check if it has already been handled
                if(_rx_buffer[_rx_cur]->lock()) { // if it wasn't, let's handle it
                    Buffer * buf = _rx_buffer[_rx_cur];
                    Rx_Desc * desc = &_rx_ring[_rx_cur];
                    Frame * frame = buf->frame();

                    // For the upper layers, size will represent the size of frame->data<T>()
                    buf->size((desc->misc & 0x00000fff) - sizeof(Header) - sizeof(CRC));

                    db<CC2538>(TRC) << "Radio::int:receive(s=" << frame->src() << ",p=" << hex << frame->header()->prot() << dec
                                     << ",d=" << frame->data<void>() << ",s=" << buf->size() << ")" << endl;

                    db<CC2538>(INF) << "Radio::handle_int:desc[" << _rx_cur << "]=" << desc << " => " << *desc << endl;

                    IC::disable(IC::irq2int(_irq));
                    if(!notify(frame->header()->prot(), buf)) // No one was waiting for this frame, so let it free for receive()
                        free(buf);
                    // TODO: this serialization is much too restrictive. It was done this way for students to play with
                    IC::enable(IC::irq2int(_irq));
                }
            }
 	}

        if(false) { // Error
            db<CC2538>(WRN) << "Radio::int:error =>";

//            if(csr0 & CSR0_MERR) { // Memory
//        	db<Radio>(WRN) << " memory";
//            }
//
//            if(csr0 & CSR0_MISS) { // Missed Frame
//        	db<Radio>(WRN) << " missed frame";
//        	_statistics.rx_overruns++;
//            }
//
//            if(csr0 & CSR0_CERR) { // Collision
//        	db<Radio>(WRN) << " collision";
//        	_statistics.collisions++;
//            }
//
//            if(csr0 & CSR0_BABL) { // Bable transmitter time-out
//        	db<Radio>(WRN) << " overrun";
//        	_statistics.tx_overruns++;
//            }

            db<CC2538>(WRN) << endl;
        }

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

__END_SYS

#endif
