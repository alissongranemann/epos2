// EPOS TI CC2538 IEEE 802.15.4 NIC Mediator Implementation


#include <machine/cortex_m/machine.h>
#include <machine/cortex_m/cc2538.h>
#include <utility/malloc.h>
#include <utility/random.h>

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
    db<CC2538>(TRC) << "CC2538::send(s=" << address() << ",d=" << dst << ",p=" << hex << type << dec << ",d=" << data << ",s=" << size << ")" << endl;

    Phy_Frame frame;
    MAC::marshal(&frame, address(), dst, type, data, size); // size adjustments are handled by the MAC
    copy_to_nic(&frame);

    db<CC2538>(INF) << "CC2538::send:frame=" << &frame << " => " << frame << endl;

    size = MAC::send();

    if(size) {
        _statistics.tx_packets++;
        _statistics.tx_bytes += size;
    } else
        db<CC2538>(WRN) << "CC2538::send(s=" << address() << ",d=" << dst << ",p=" << hex << type << dec << ",d=" << data << ",s=" << size << ")" << " => failed!" << endl;

    return size;
}


int CC2538::receive(Address * src, Type * type, void * data, unsigned int size)
{
    db<CC2538>(TRC) << "CC2538::receive(s=" << *src << ",p=" << hex << *type << dec << ",d=" << data << ",s=" << size << ") => " << endl;

    Phy_Frame frame;
    MAC::receive();
    copy_from_nic(&frame);
    Address dst;
    size = MAC::unmarshal(&frame, src, &dst, type, data, size);

    _statistics.rx_packets++;
    _statistics.rx_bytes += size;

    db<CC2538>(INF) << "CC2538::receive:frame=" << &frame  << " => " << frame << endl;

    return size;
}


CC2538::Buffer * CC2538::alloc(NIC * nic, const Address & dst, const Type & type, unsigned int once, unsigned int always, unsigned int payload)
{
    db<CC2538>(TRC) << "CC2538::alloc(s=" << address() << ",d=" << dst << ",p=" << hex << type << dec << ",on=" << once << ",al=" << always << ",ld=" << payload << ")" << endl;

    // Initialize the buffer and assemble the Ethernet Frame Header
    return new (SYSTEM) Buffer(nic, once + always + payload, once + always + payload); // the last parameter is passed to Phy_Frame as the length
}


int CC2538::send(Buffer * buf)
{
    db<CC2538>(TRC) << "CC2538::send(buf=" << buf << ")" << endl;
    db<CC2538>(INF) << "CC2538::send:frame=" << buf->frame() << " => " << *(buf->frame()) << endl;

    copy_to_nic(buf->frame());
    unsigned int size = MAC::send();

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
    db<CC2538>(TRC) << "CC2538::handle_int()" << endl;

    Reg32 irqrf0 = sfr(RFIRQF0);
    Reg32 irqrf1 = sfr(RFIRQF1);
    Reg32 errf = sfr(RFERRF);
    sfr(RFIRQF0) = 0;
    sfr(RFIRQF1) = 0;
    sfr(RFERRF) = 0;
    db<CC2538>(INF) << "CC2538::handle_int:RFIRQF0=" << hex << irqrf0 << endl;
    db<CC2538>(INF) << "CC2538::handle_int:RFIRQF1=" << hex << irqrf1 << endl;
    db<CC2538>(INF) << "CC2538::handle_int:RFERRF=" << hex << errf << endl;

    if(irqrf0 & INT_FIFOP) { // Frame received
        sfr(RFIRQF0) &= ~INT_FIFOP;
        if((xreg(RXFIFOCNT) > 0)) {
            db<CC2538>(TRC) << "CC2538::handle_int:receive()" << endl;
            Buffer * buf = new (SYSTEM) Buffer(0);
            Phy_Frame * frame = buf->frame();
            copy_from_nic(frame);
            buf->size(frame->length() - (sizeof(Header) + sizeof(CRC) - sizeof(Phy_Header))); // Phy_Header is included in Header, but is already discounted in frame_length
            db<CC2538>(TRC) << "CC2538::handle_int:receive(b=" << buf << ") => " << *buf << endl;
            if(!notify(0, buf)) // No one was waiting for this frame, so let it free for receive()
                free(buf);
        }
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
