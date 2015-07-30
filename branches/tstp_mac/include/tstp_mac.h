// EPOS TSTP MAC Mediator Common Package

#include <nic.h>

#ifndef __tstp_mac_h
#define __tstp_mac_h

#include <ieee802_15_4.h>
#include <cpu.h>
#include <utility/list.h>
#include <utility/observer.h>

__BEGIN_SYS

class TSTP_MAC: private NIC_Common
{
protected:
//    static const unsigned int HEADER_SIZE = 14;

public:
    static const unsigned int MTU = 127;
    // TODO: Geographic address
    typedef IF<Traits<TSTP_MAC>::geographic, void, NIC_Common::Address<2>>::Result Address;
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;    

    // Frame types
    enum Frame_Type
    {
        BEACON  = 0,
        DATA    = 1,
        ACK     = 2,
        MAC_CMD = 3,
    };
    enum Addressing_Mode
    {
        ADDR_MODE_NOT_PRESENT = 0,
        ADDR_MODE_SHORT_ADDR  = 2,
        ADDR_MODE_EXT_ADDR    = 4,
    };
    enum
    {
        PAN_ID_BROADCAST = 0xffff
    };

    typedef unsigned short Protocol;
    enum
    {
        IP     = 0x0800,
        ARP    = 0x0806,
        RARP   = 0x8035,
        ELP    = 0x8888,
        PTP    = 0x88F7
    };


    typedef unsigned char Data[MTU];
    typedef NIC_Common::CRC16 CRC;

    // The IEEE 802.15.4 PHR
    class Phy_Header
    {
    public:
        Phy_Header() {};
        Phy_Header(Reg8 len) : _frame_length(len) {};

        Reg8 frame_length() const { return _frame_length; }
        void frame_length(Reg8 len) { _frame_length = len; }
    
    protected:
        Reg8 _frame_length;        
    } __attribute__((packed, may_alias));

    class Microframe : public Phy_Header
    {
    public:
        Microframe(Reg16 id) : 
        Phy_Header(sizeof(Microframe) - sizeof(Phy_Header)),
        _all_listen((!Traits<TSTP_MAC>::geographic) || (Traits<TSTP_MAC>::geographic && Traits<TSTP_MAC>::is_sink)),
        _count(Traits<TSTP_MAC>::n_microframes - 1),
        _last_hop_distance(Traits<TSTP_MAC>::geographic ?  : 0),
       _id(id) 
       {};

    protected:
        unsigned _all_listen : 1;
        unsigned _count : 15;
        unsigned _last_hop_distance : 32;
        unsigned _id : 15;
        CRC _crc;
    } __attribute__((packed, may_alias));

    typedef IEEE802_15_4::Header Header;
    typedef IEEE802_15_4::Frame Frame;
    typedef IEEE802_15_4::Buffer Buffer;

    typedef Frame PDU;

    //class Frame: public Phy_Header
    //{
    //public:
    //    Frame() {}
    //    Frame(const void * data, Reg8 size) : Phy_Header(size+sizeof(CRC))
    //    {
    //        memcpy(_data, data, size);
    //    }

    //    Phy_Header * header() { return this; }

    //    template<typename T>
    //    T * data() { return reinterpret_cast<T *>(&_data); }

    //    friend Debug & operator<<(Debug & db, const Frame & f) {
    //        db << "{" << f._data << "," << f._crc << "}";
    //        return db;
    //    }
    //    
    //protected:
    //    Data _data;
    //    CRC _crc;
    //} __attribute__((packed, may_alias));

    //typedef Frame PDU;

    //// Buffers used to hold frames across a zero-copy network stack
    //class Buffer: private Frame
    //{
    //public:
    //    typedef Simple_List<Buffer> List;
    //    typedef List::Element Element;

    //public:
    //    Buffer(void * back): _lock(false), _nic(0), _back(back), _size(sizeof(Frame)), _link(this) {}

    //    Frame * frame() { return this; }

    //    bool lock() { return !CPU::tsl(_lock); }
    //    void unlock() { _lock = 0; }

    //    NIC * nic() const { return _nic; }
    //    void nic(NIC * n) { _nic = n; }

    //    template<typename T>
    //    T * back() const { return reinterpret_cast<T *>(_back); }

    //    unsigned int size() const { return _size; }
    //    void size(unsigned int s) { _size = s; }

    //    Element * link() { return &_link; }

    //    friend Debug & operator<<(Debug & db, const Buffer & b) {
    //        db << "{nc=" << b._nic << ",lk=" << b._lock << ",sz=" << b._size << ",bl=" << b._back << "}";
    //        return db;
    //    }

    //private:
    //    volatile bool _lock;
    //    NIC * _nic;
    //    void * _back;
    //    unsigned int _size;
    //    Element _link;
    //};
//    } __attribute__((packed, may_alias));


public:
    // Observers of a protocol get a also a pointer to the received buffer
    typedef Data_Observer<Buffer, Protocol> Observer;
    typedef Data_Observed<Buffer, Protocol> Observed;


    // Meaningful statistics for Ethernet
    struct Statistics: public NIC_Common::Statistics
    {
        Statistics(): rx_overruns(0), tx_overruns(0), frame_errors(0), carrier_errors(0), collisions(0) {}

        friend Debug & operator<<(Debug & db, const Statistics & s) {
            db << "{rxp=" << s.rx_packets
               << ",rxb=" <<  s.rx_bytes
               << ",rxorun=" <<  s.rx_overruns
               << ",txp=" <<  s.tx_packets
               << ",txb=" <<  s.tx_bytes
               << ",txorun=" <<  s.tx_overruns
               << ",frm=" <<  s.frame_errors
               << ",car=" <<  s.carrier_errors
               << ",col=" <<  s.collisions
               << "}";
            return db;
        }
        
        unsigned int rx_overruns;
        unsigned int tx_overruns;
        unsigned int frame_errors;
        unsigned int carrier_errors;
        unsigned int collisions;
    };

/*
    virtual void listen() = 0;
    virtual void stop_listening() = 0;
    */

protected:
    TSTP_MAC() {}

public:
    static const unsigned int mtu() { return MTU; }
    static const Address broadcast() { return Address::BROADCAST; }
};

__END_SYS

#endif
