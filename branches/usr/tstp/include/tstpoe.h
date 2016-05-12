// EPOS TSTP over Ethernet Protocol Declarations

#include <nic.h>
#include <tstp.h>
#include <rtc.h>

#ifndef __tstpoe_h
#define __tstpoe_h

__BEGIN_SYS

class TSTPOE: private TSTP_Common, private NIC::Observer
{
    friend class System;
    template<int unit> friend void call_init();

public:
    static const unsigned int TX_DELAY = 0;

    // TSTP Packet
    static const unsigned int MTU = 1500 - sizeof(Header);
    template<Scale S>
    class _Packet: public Header
    {
    private:
        typedef unsigned char Data[MTU];

    public:
        _Packet() {}

        Header * header() { return this; }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const _Packet & p) {
            db << "{h=" << reinterpret_cast<const Header &>(p) << ",d=" << p._data << "}";
            return db;
        }

    private:
        Data _data;
    } __attribute__((packed));
    typedef _Packet<SCALE> Packet;


    // Buffers received from the NIC
    typedef NIC::Buffer Buffer;

    typedef NIC_Observing_Condition Condition;
    typedef Data_Observer<Buffer, Condition> Observer;
    typedef Data_Observed<Buffer, Condition> Observed;

protected:
    template<unsigned int UNIT = 0>
    TSTPOE(unsigned int nic = UNIT): _nic(nic) {
        db<TSTPOE>(TRC) << "TSTPOE::TSTPOE(nic=" << &_nic << ") => " << this << endl;

        _nic.attach(this, NIC::TSTP);
    }

public:
    ~TSTPOE() {
        db<TSTPOE>(TRC) << "TSTPOE::~TSTPOE(nic=" << &_nic << ") => " << this << endl;

        _nic.detach(this, NIC::TSTP);
    }

    static Buffer * alloc(unsigned int payload)
    {
        db<TSTPOE>(TRC) << "TSTPOE::alloc(pl=" << payload << ")" << endl;

        return nic()->alloc(nic(), NIC::Address::BROADCAST, NIC::TSTP, 0, 0, payload);
    }

    static int send(Buffer * buf);

    static Coordinates here() { return (nic()->address())[5] % 2 ? Coordinates(0, 0, 0) : Coordinates(10, 10, 10); }

    static NIC * nic() { return &(_networks[0]->_nic); }
    static const unsigned int mtu() { return MTU; }

    static void attach(Observer * obs, Condition c) { _observed.attach(obs,c); }
    static void detach(Observer * obs, Condition c) { _observed.detach(obs,c); }
    static bool notify(Buffer * buf, Condition c) { return _observed.notify(c, buf); }

private:
    void update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf);

    static void init(unsigned int unit) {
         db<Init, TSTP>(TRC) << "TSTPOE::init(u=" << unit << ")" << endl;

         _networks[unit] = new (SYSTEM) TSTPOE(unit);
     }

protected:
    NIC _nic;

    static TSTPOE * _networks[Traits<NIC>::UNITS];
    static Observed _observed; // shared by all TSTPOE instances, so the default for binding on a unit is for all NICs
};

__END_SYS

#endif
