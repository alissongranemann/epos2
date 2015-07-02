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

void CC2538::address(const Address & address) 
{ 
    _address[0] = address[0]; 
    _address[1] = address[1]; 
    ffsm(SHORT_ADDR0) = _address[0];
    ffsm(SHORT_ADDR1) = _address[1];
}

void CC2538::listen() 
{ 
    // Clear interrupts
    sfr(RFIRQF0) = 0;
    sfr(RFIRQF1) = 0;
    // Enable device interrupts
    xreg(RFIRQM0) = ~0;
    xreg(RFIRQM1) = ~0;
    // Issue the listen command
    sfr(RFST) = ISRXON; 
}

void CC2538::stop_listening() 
{ 
    // Disable device interrupts
    xreg(RFIRQM0) = 0;
    xreg(RFIRQM1) = 0;
    // Issue the OFF command
    sfr(RFST) = ISRFOFF; 
}

void CC2538::set_channel(unsigned int channel)
{
    if((channel < 11) || (channel > 26)) return;
	/*
	   The carrier frequency is set by programming the 7-bit frequency word in the FREQ[6:0] bits of the
	   FREQCTRL register. Changes take effect after the next recalibration. Carrier frequencies in the range
	   from 2394 to 2507 MHz are supported. The carrier frequency f C , in MHz, is given by 
       f C = (2394 + FREQCTRL.FREQ[6:0]) MHz, and is programmable in 1-MHz steps.
	   IEEE 802.15.4-2006 specifies 16 channels within the 2.4-GHz band. These channels are numbered 11
	   through 26 and are 5 MHz apart. The RF frequency of channel k is given by Equation 1.
	   f c = 2405 + 5(k –11) [MHz] k [11, 26]
	   (1)
	   For operation in channel k, the FREQCTRL.FREQ register should therefore be set to
	   FREQCTRL.FREQ = 11 + 5 (k – 11).
    */
    xreg(FREQCTRL) = 11+5*(channel-11);
}

int CC2538::send(const Address & dst, const Protocol & prot, const void * data, unsigned int size)
{
    // Wait for the buffer to become free and seize it
    for(bool locked = false; !locked; ) {
        locked = _tx_buffer[_tx_cur]->lock();
        if(!locked) ++_tx_cur;
    }
        
    sfr(RFST) = ISRFOFF; // Turn off the receiver

    Buffer * buf = _tx_buffer[_tx_cur];

    db<CC2538>(TRC) << "CC2538::send(s=" << _address << ",d=" << dst << ",p=" << prot
                     << ",d=" << data << ",s=" << size << ")" << endl;

    char * f = reinterpret_cast<char *>(new (buf->frame()) Frame(_address, dst, prot, data, size));
    // Assemble the 802.15.4 frame
    // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
    // For now, we'll just copy using the RFDATA register
    sfr(RFST) = ISFLUSHTX; // Clear TXFIFO
    for(int i=0; i<f[0]+1; i++) // First field is length of MAC
        sfr(RFDATA) = f[i];

    // Trigger an immediate send poll
    _send_and_wait();

    _statistics.tx_packets++;
    _statistics.tx_bytes += size;

    db<CC2538>(INF) << "CC2538::send done" << endl;

    buf->unlock();

    return size;
}


int CC2538::receive(Address * src, Protocol * prot, void * data, unsigned int size)
{
    db<CC2538>(TRC) << "CC2538::receive(s=" << *src << ",p=" << hex << *prot << dec
                     << ",d=" << data << ",s=" << size << ") => " << endl;

    // Wait for a received frame and seize it
    for(bool locked = false; !locked; ) {
        locked = _rx_buffer[_rx_cur]->lock();
        if(!locked) ++_rx_cur;
    }

    Buffer * buf = _rx_buffer[_rx_cur];

    // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
    // For now, we'll just copy using the RFDATA register
    //char * cbuf = reinterpret_cast<char *>(buf);
    //int end = xreg(RXFIFOCNT);
    //for(int i=0; i<end; i++) // First field is length of MAC
    //    cbuf[i] = sfr(RFDATA);
    //sfr(RFST) = ISFLUSHRX; // Clear RXFIFO

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

    db<CC2538>(INF) << "CC2538::receive done" << endl;

    int tmp = buf->size();

    buf->unlock();

    ++_rx_cur %= RX_BUFS;

    return tmp;
}


// Allocated buffers must be sent or release IN ORDER as assumed by the Radio
CC2538::Buffer * CC2538::alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload)
{
    db<CC2538>(TRC) << "CC2538::alloc(s=" << _address << ",d=" << dst << ",p=" << hex << prot << dec << ",on=" << once << ",al=" << always << ",ld=" << payload << ")" << endl;

    int max_data = MTU - always;

    if((payload + once) / max_data > TX_BUFS) {
        db<CC2538>(WRN) << "CC2538::alloc: sizeof(Network::Packet::Data) > sizeof(NIC::Frame::Data) * TX_BUFS!" << endl;
        return 0;
    }

    Buffer::List pool;

    // Calculate how many frames are needed to hold the transport PDU and allocate enough buffers
    for(int size = once + payload; size > 0; size -= max_data) {
        // Wait for the next buffer to become free and seize it
        for(bool locked = false; !locked; ) {
            locked = _tx_buffer[_tx_cur]->lock();
            if(!locked) ++_tx_cur;
        }
        Buffer * buf = _tx_buffer[_tx_cur];

        // Initialize the buffer and assemble the Ethernet Frame Header
        new (buf) Buffer(nic, _address, dst, prot, (size > max_data) ? MTU : size + always);

        db<CC2538>(INF) << "CC2538::alloc[" << _tx_cur << "]" << endl;

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

        db<CC2538>(TRC) << "CC2538::send(buf=" << buf << ")" << endl;

        // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
        // For now, we'll just copy using the RFDATA register
        char * f = reinterpret_cast<char *>(buf->frame());
        sfr(RFST) = ISFLUSHTX; // Clear TXFIFO
        for(int i=0; i<f[0]+1; i++) // First field is length of MAC
        sfr(RFDATA) = f[i];

        // Trigger an immediate send poll
        _send_and_wait();

        size += buf->size();

        _statistics.tx_packets++;
        _statistics.tx_bytes += buf->size();

        db<CC2538>(INF) << "CC2538::send" << endl;

        buf->unlock();
    }

    return size;
}


void CC2538::free(Buffer * buf)
{
    db<CC2538>(TRC) << "CC2538::free(buf=" << buf << ")" << endl;

    for(Buffer::Element * el = buf->link(); el; el = el->next()) {
        buf = el->object();

        _statistics.rx_packets++;
        _statistics.rx_bytes += buf->size();

        // Release the buffer to the OS
        buf->unlock();

        db<CC2538>(INF) << "CC2538::free" << endl;
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
    Reg32 irqrf0 = sfr(RFIRQF0);
    Reg32 irqrf1 = sfr(RFIRQF1);

    if(irqrf0 & INT_FIFOP) { // Frame received
        // Note that ISRs in EPOS are reentrant, that's why locking was carefully made atomic
        // Therefore, several instances of this code can compete to handle received buffers
        sfr(RFIRQF0) &= ~INT_FIFOP;

        // NIC received a frame in _rx_buffer[_rx_cur], let's check if it has already been handled
        if(_rx_buffer[_rx_cur]->lock()) { // if it wasn't, let's handle it
            Buffer * buf = _rx_buffer[_rx_cur];
            Frame * frame = buf->frame();

            // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
            // For now, we'll just copy using the RFDATA register
            char * cbuf = reinterpret_cast<char *>(buf);
            int end = xreg(RXFIFOCNT);
            for(int i=0; i<end; i++) // First field is length of MAC
                cbuf[i] = sfr(RFDATA);
            sfr(RFST) = ISFLUSHRX; // Clear RXFIFO

            // For the upper layers, size will represent the size of frame->data<T>()
            buf->size(buf->frame()->frame_length() - sizeof(Header) - sizeof(CRC) + sizeof(Phy_Header)); // Phy_Header is included in Header, but is already discounted in frame_length

            db<CC2538>(TRC) << "CC2538::int:receive(s=" << frame->src() << ",p=" << hex << frame->header()->prot() << dec
                << ",d=" << frame->data<void>() << ",s=" << buf->size() << ")" << endl;

            db<CC2538>(INF) << "CC2538::handle_int[" << _rx_cur << "]" << endl;

            IC::disable(_irq);
            if(!notify(frame->header()->prot(), buf)) // No one was waiting for this frame, so let it free for receive()
                free(buf);
            // TODO: this serialization is much too restrictive. It was done this way for students to play with
            IC::enable(_irq);
        }
    }
        
    db<CC2538>(TRC) << "CC2538::int: " << endl << "RFIRQF0 = " << irqrf0 << endl;
    if(irqrf0 & INT_RXMASKZERO) db<CC2538>(TRC) << "RXMASKZERO" << endl;
    if(irqrf0 & INT_RXPKTDONE) db<CC2538>(TRC) << "RXPKTDONE" << endl;
    if(irqrf0 & INT_FRAME_ACCEPTED) db<CC2538>(TRC) << "FRAME_ACCEPTED" << endl;
    if(irqrf0 & INT_SRC_MATCH_FOUND) db<CC2538>(TRC) << "SRC_MATCH_FOUND" << endl;
    if(irqrf0 & INT_SRC_MATCH_DONE) db<CC2538>(TRC) << "SRC_MATCH_DONE" << endl;
    if(irqrf0 & INT_FIFOP) db<CC2538>(TRC) << "FIFOP" << endl;
    if(irqrf0 & INT_SFD) db<CC2538>(TRC) << "SFD" << endl;
    if(irqrf0 & INT_ACT_UNUSED) db<CC2538>(TRC) << "ACT_UNUSED" << endl;

    db<CC2538>(TRC) << "RFIRQF1 = " << irqrf1 << endl;
    if(irqrf1 & INT_CSP_WAIT) db<CC2538>(TRC) << "CSP_WAIT" << endl;
    if(irqrf1 & INT_CSP_STOP) db<CC2538>(TRC) << "CSP_STOP" << endl;
    if(irqrf1 & INT_CSP_MANINT) db<CC2538>(TRC) << "CSP_MANINT" << endl;
    if(irqrf1 & INT_RFIDLE) db<CC2538>(TRC) << "RFIDLE" << endl;
    if(irqrf1 & INT_TXDONE) db<CC2538>(TRC) << "TXDONE" << endl;
    if(irqrf1 & INT_TXACKDONE) db<CC2538>(TRC) << "TXACKDONE" << endl;


    if(false) { // Error
        db<CC2538>(WRN) << "CC2538::int:error =>";
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
