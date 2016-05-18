// EPOS TSTP MAC Mediator Common Package

#include <nic.h>

#ifndef __tstp_mac_h
#define __tstp_mac_h

#include <tstp_common.h>
#include <utility/list.h>
#include <utility/observer.h>
#include <utility/buffer.h>

__BEGIN_SYS

class TSTP_MAC: private NIC::Common, public TSTP_Common
{
public:
    static const unsigned int TX_DELAY = 0; // TODO

    typedef NIC_Common::CRC16 CRC;
    typedef CPU::Reg16 Frame_ID;
    typedef TSTP_Common::Microsecond Time;

    // Just to comply with EPOS' NIC interface
    typedef NIC_Common::Address<1> Address;
    enum Protocol { TSTP   = 0x8401, };

    // TSTP MAC Frame
    static const unsigned int MTU = 127 - sizeof(CRC);

    class Frame: public Header
    {
    private:
        typedef unsigned char Data[MTU];

    public:
        _Frame() {}

        Header * header() { return this; }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const _Frame & p) {
            db << "{h=" << reinterpret_cast<const Header &>(p) << ",d=" << p._data << "}";
            return db;
        }

    private:
        Data _data;
        CRC _crc;
    } __attribute__((packed));

    typedef Frame PDU;

    // Buffers used to hold frames across a zero-copy network stack
    class Buffer : public _UTIL::Buffer<NIC, TSTP_MAC::Frame, void> {
        friend class TSTP_MAC;

        typedef _UTIL::Buffer<NIC, TSTP_MAC::Frame> Base;
        typedef Ordered_List<Buffer, TSTP_MAC::Time> List;
        typedef List::Element Element;

    public:
        Buffer(void * s) : Base(s), _tx_link(this), _id(0) { }
        Buffer(NIC * n, unsigned int s) : Base(n, s), _tx_link(this), _id(0) { }
        template<typename ... Tn>
        Buffer(NIC * n, unsigned int s, Tn ... an): Base(n, s, an ...), _tx_link(this), _id(0) {}

        Element * tx_link() { return &_tx_link; }

        void set_id() { _id = (Random::random() & 0x7fff); } // TODO

        void id(TSTP_MAC::Frame_ID i) { _id = i; }
        TSTP_MAC::Frame_ID id() const { return _id; }

    private:
        Element _tx_link;
        TSTP_MAC::Frame_ID _id;
    };

    // Observers of a protocol get a also a pointer to the received buffer
    typedef Data_Observer<Buffer, Protocol> Observer;
    typedef Data_Observed<Buffer, Protocol> Observed;

protected:
    typedef unsigned char Count;

    class Microframe {
    public:
        Microframe(bool all_listen, const Frame_ID & id, const Count & count, const Distance & hint) : 
            _al_id((id & 0x7fff) | (static_cast<unsigned int>(all_listen) << 15)), _count(count), _hint(hint) {}

        Microframe(bool all_listen, const Frame_ID & id, const Count & count) : 
            _al_id((id & 0x7fff) | (static_cast<unsigned int>(all_listen) << 15)), _count(count), _hint(0) {}

        Count count() const { return _count; }
        Count dec_count() { Count ret = _count--; return ret; }

        Frame_ID id() const { return _al_id & 0x7fff; }
        void id(Frame_ID id) { _al_id  = all_listen() | (id & 0x7fff); }

        void all_listen(bool all_listen) { _al_id = id() | (1 << 15); }
        bool all_listen() const { return _al_id & ~(0x7fff); }

        Distance hint() const { return _hint; }
        void hint(const Distance & h) { _hint = h; }

    private:
        unsigned short _al_id; // all_listen : 1 
                               // id : 15
        Count _count;
        Distance _hint;
        CRC _crc;
    } __attribute__((packed));

    enum STATE {
        CHECK_TX_SCHEDULE,
        SLEEP_S,
        RX_MF,
        SLEEP_UNTIL_DATA,
        RX_DATA,
        OFFSET,
        CCA,
        TX_MF,
        TX_DATA        
    };

    typedef Simple_Relative_List<Buffer, Time> TX_Schedule;
};

__END_SYS

#endif
