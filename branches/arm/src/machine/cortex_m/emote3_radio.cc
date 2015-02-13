// EPOS PC AMD PCNet II (Am79C970A) Ethernet NIC Mediator Implementation

#include <machine/cortex_m/machine.h>
#include <machine/cortex_m/emote3_radio.h>
#include <utility/malloc.h>
#include <alarm.h>

__BEGIN_SYS

// Class attributes
eMote3_Radio::Device eMote3_Radio::_devices[UNITS];


// Methods
eMote3_Radio::~eMote3_Radio()
{
    db<eMote3_Radio>(TRC) << "~eMote3_Radio(unit=" << _unit << ")" << endl;
}

void eMote3_Radio::listen() 
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

void eMote3_Radio::stop_listening() 
{ 
    // Disable device interrupts
    xreg(RFIRQM0) = 0;
    xreg(RFIRQM1) = 0;
    // Issue the OFF command
    sfr(RFST) = ISRFOFF; 
}

void eMote3_Radio::set_channel(unsigned int channel)
{
    if((channel < 11) || (channel > 26)) return;
	/*
	   The carrier frequency is set by programming the 7-bit frequency word in the FREQ[6:0] bits of the
	   FREQCTRL register. Changes take effect after the next recalibration. Carrier frequencies in the range
	   from 2394 to 2507 MHz are supported. The carrier frequency f C , in MHz, is given by f C = (2394 +
	   FREQCTRL.FREQ[6:0]) MHz, and is programmable in 1-MHz steps.
	   IEEE 802.15.4-2006 specifies 16 channels within the 2.4-GHz band. These channels are numbered 11
	   through 26 and are 5 MHz apart. The RF frequency of channel k is given by Equation 1.
	   f c = 2405 + 5(k –11) [MHz] k [11, 26]
	   (1)
	   For operation in channel k, the FREQCTRL.FREQ register should therefore be set to
	   FREQCTRL.FREQ = 11 + 5 (k – 11).
    */
    xreg(FREQCTRL) = 11+5*(channel-11);
}

int eMote3_Radio::send(const Address & dst, const Protocol & prot, const void * data, unsigned int size)
{
    // Wait for the buffer to become free and seize it
    for(bool locked = false; !locked; ) {
        locked = _tx_buffer[_tx_cur]->lock();
        if(!locked) ++_tx_cur;
    }

    Buffer * buf = _tx_buffer[_tx_cur];

    db<eMote3_Radio>(TRC) << "eMote3_Radio::send(s=" << _address << ",d=" << dst << ",p=" << prot
                     << ",d=" << data << ",s=" << size << ")" << endl;

    char * f = reinterpret_cast<char *>(new (buf->frame()) Frame(_address, dst, prot, data, size));
    // Assemble the 802.15.4 frame
    // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
    // For now, we'll just copy using the RFDATA register
    sfr(RFST) = ISFLUSHTX; // Clear TXFIFO
    for(int i=0; i<f[0]+1; i++) // First field is length of MAC
    {
        kout << (int)f[i] << endl;
        sfr(RFDATA) = f[i];
    }

    // Trigger an immediate send poll
    _cmd_send_now();

    _statistics.tx_packets++;
    _statistics.tx_bytes += size;

    // Wait for packet to be sent
    while(!_tx_done());

    db<eMote3_Radio>(INF) << "eMote3_Radio::send done" << endl;

    buf->unlock();

    return size;
}


int eMote3_Radio::receive(Address * src, Protocol * prot, void * data, unsigned int size)
{
    db<eMote3_Radio>(TRC) << "eMote3_Radio::receive(s=" << *src << ",p=" << hex << *prot << dec
                     << ",d=" << data << ",s=" << size << ") => " << endl;

    // Wait for a received frame and seize it
    for(bool locked = false; !locked; ) {
        locked = _rx_buffer[_rx_cur]->lock();
        if(!locked) ++_rx_cur;
    }

    // Wait for a complete frame to be received
    while(!_rx_done());

    Buffer * buf = _rx_buffer[_rx_cur];

    // TODO: Memory in the fifos is padded: you can only write one byte every 4bytes.
    // For now, we'll just copy using the RFDATA register
    char * cbuf = reinterpret_cast<char *>(buf);
    int end = xreg(RXFIFOCNT);
    for(int i=0; i<end; i++) // First field is length of MAC
        cbuf[i] = sfr(RFDATA);
    sfr(RFST) = ISFLUSHRX; // Clear RXFIFO

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

    db<eMote3_Radio>(INF) << "eMote3_Radio::receive done" << endl;

    int tmp = buf->size();

    buf->unlock();

    ++_rx_cur %= RX_BUFS;

    return tmp;
}


// Allocated buffers must be sent or release IN ORDER as assumed by the Radio
eMote3_Radio::Buffer * eMote3_Radio::alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload)
{
    db<eMote3_Radio>(TRC) << "eMote3_Radio::alloc(s=" << _address << ",d=" << dst << ",p=" << hex << prot << dec << ",on=" << once << ",al=" << always << ",ld=" << payload << ")" << endl;

    int max_data = MTU - always;

    if((payload + once) / max_data > TX_BUFS) {
        db<eMote3_Radio>(WRN) << "eMote3_Radio::alloc: sizeof(Network::Packet::Data) > sizeof(NIC::Frame::Data) * TX_BUFS!" << endl;
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

        db<eMote3_Radio>(INF) << "eMote3_Radio::alloc[" << _tx_cur << "]" << endl;

        ++_tx_cur %= TX_BUFS;

        pool.insert(buf->link());
    }

    return pool.head()->object();
}


int eMote3_Radio::send(Buffer * buf)
{
    unsigned int size = 0;

    for(Buffer::Element * el = buf->link(); el; el = el->next()) {
        buf = el->object();

        db<eMote3_Radio>(TRC) << "eMote3_Radio::send(buf=" << buf << ")" << endl;

        // Trigger an immediate send poll
        _cmd_send_now();

        size += buf->size();

        _statistics.tx_packets++;
        _statistics.tx_bytes += buf->size();

        db<eMote3_Radio>(INF) << "eMote3_Radio::send" << endl;

        // Wait for packet to be sent and unlock the respective buffer
        while(!_tx_done());
        buf->unlock();
    }

    return size;
}


void eMote3_Radio::free(Buffer * buf)
{
    db<eMote3_Radio>(TRC) << "eMote3_Radio::free(buf=" << buf << ")" << endl;

    for(Buffer::Element * el = buf->link(); el; el = el->next()) {
        buf = el->object();

        _statistics.rx_packets++;
        _statistics.rx_bytes += buf->size();

        // Release the buffer to the OS
        buf->unlock();

        db<eMote3_Radio>(INF) << "eMote3_Radio::free" << endl;
    }
}


void eMote3_Radio::reset()
{
    db<eMote3_Radio>(TRC) << "eMote3_Radio::reset()" << endl;


    // Reset statistics
    new (&_statistics) Statistics;
}


void eMote3_Radio::handle_int()
{
    if(sfr(RFIRQF0) & FIFOP) { // Frame received
        // Clear interrupt
        sfr(RFIRQF0) &= ~FIFOP;

        // Note that ISRs in EPOS are reentrant, that's why locking was carefully made atomic
        // Therefore, several instances of this code can compete to handle received buffers

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

            db<eMote3_Radio>(TRC) << "eMote3_Radio::int:receive(s=" << frame->src() << ",p=" << hex << frame->header()->prot() << dec
                << ",d=" << frame->data<void>() << ",s=" << buf->size() << ")" << endl;

            db<eMote3_Radio>(INF) << "eMote3_Radio::handle_int[" << _rx_cur << "]" << endl;

            IC::disable(_irq);
            if(!notify(frame->header()->prot(), buf)) // No one was waiting for this frame, so let it free for receive()
                free(buf);
            // TODO: this serialization is much too restrictive. It was done this way for students to play with
            IC::enable(_irq);
        }
    }

    if(false) { // Error
        db<eMote3_Radio>(WRN) << "eMote3_Radio::int:error =>";

//            if(csr0 & CSR0_MERR) { // Memory
//        	db<eMote3_Radio>(WRN) << " memory";
//            }
//
//            if(csr0 & CSR0_MISS) { // Missed Frame
//        	db<eMote3_Radio>(WRN) << " missed frame";
//        	_statistics.rx_overruns++;
//            }
//
//            if(csr0 & CSR0_CERR) { // Collision
//        	db<eMote3_Radio>(WRN) << " collision";
//        	_statistics.collisions++;
//            }
//
//            if(csr0 & CSR0_BABL) { // Bable transmitter time-out
//        	db<eMote3_Radio>(WRN) << " overrun";
//        	_statistics.tx_overruns++;
//            }

        db<eMote3_Radio>(WRN) << endl;
    }
}


void eMote3_Radio::int_handler(const IC::Interrupt_Id & interrupt)
{
    eMote3_Radio * dev = get_by_interrupt(interrupt);

    db<eMote3_Radio>(TRC) << "eMote3_Radio::int_handler(int=" << interrupt << ",dev=" << dev << ")" << endl;

    if(!dev)
        db<eMote3_Radio>(WRN) << "eMote3_Radio::int_handler: handler not assigned!" << endl;
    else
        dev->handle_int();
}

__END_SYS
