// EPOS Buffer Declarations
// This Buffer was designed to move data across a zero-copy communication stack, but can be used for several other purposes

#ifndef __buffer_h
#define __buffer_h

#include <cpu.h>

__BEGIN_UTIL

template<typename Owner, typename Data, typename Shadow = void>
class Buffer: private Data
{
public:
    typedef Simple_List<Buffer<Owner, Data, Shadow> > List;
    typedef typename List::Element Element;

public:
    // This constructor is meant to be used at initialization time to correlate shadow data structures (e.g. NIC ring buffers)
    Buffer(Shadow * s): _lock(false), _owner(0), _shadow(s), _size(sizeof(Data)), _link1(this), _link2(this) {}

    // These constructors are used whenever a Buffer receives new data
    Buffer(Owner * o, unsigned int s): _lock(false), _owner(o), _size(s), _link1(this), _link2(this) {}
    template<typename ... Tn>
    Buffer(Owner * o, unsigned int s, Tn ... an): Data(an ...), _lock(false), _owner(o), _size(s), _link1(this), _link2(this) {}

    Data * data() { return this; }
    Data * frame() { return data(); }
    Data * message() { return data(); }

    bool lock() { return !CPU::tsl(_lock); }
    void unlock() { _lock = 0; }

    Owner * owner() const { return _owner; }
    Owner * nic() const { return owner(); }
    void owner(Owner * o) { _owner = o; }
    void nic(Owner * o) { owner(o); }

    Shadow * shadow() const { return _shadow; }
    Shadow * back() const { return shadow(); }

    unsigned int size() const { return _size; }
    void size(unsigned int s) { _size = s; }

    Element * link1() { return &_link1; }
    Element * link() { return link1(); }
    Element * lint() { return link1(); }
    Element * link2() { return &_link2; }
    Element * lext() { return link2(); }

    friend Debug & operator<<(Debug & db, const Buffer & b) {
        db << "{md=" << b._owner << ",lk=" << b._lock << ",sz=" << b._size << ",sd=" << b._shadow << "}";
        return db;
    }

    int rssi() const { return _rssi; }
    void rssi(int r) { _rssi = r; }
    unsigned long long sfd_time() const { return _sfd_time; }
    void sfd_time(unsigned long long t) { _sfd_time = t; }
    unsigned int id() const { return _id; }
    void id(unsigned int i) { _id = i; }
    unsigned long long offset() const { return _offset; }
    void offset(unsigned long long o) { _offset = o; }
    bool destined_to_me() const { return _destined_to_me; }
    void destined_to_me(bool d) { _destined_to_me = d; }
    unsigned long long deadline() const { return _deadline; }
    void deadline(unsigned long long t) { _deadline = t; }

private:
    volatile bool _lock;
    Owner * _owner;
    Shadow * _shadow;
    unsigned int _size;
    Element _link1;
    Element _link2;

    int _rssi;
    unsigned long long _sfd_time;
    unsigned int _id;
    unsigned long long _offset;
    bool _destined_to_me;
    unsigned long long _deadline;
};

__END_UTIL

#endif
