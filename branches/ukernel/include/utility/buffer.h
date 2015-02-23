#ifndef __buffer_h
#define	__buffer_h

#include <utility/list.h>
#include <cpu.h>

__BEGIN_UTIL

template<typename T>
class Buffer
{
public:
    typedef List_Elements::Doubly_Linked<Buffer> Element;

public:
    Buffer(T data): _lock(false), _data(data), _link(this) {}

    Buffer(): _lock(false), _link(this) {}

    bool lock() { return !CPU::tsl(_lock); }
    void unlock() { _lock = 0; }

    T data() const { return _data; }
    void data(T data) { _data = data; }

    Element* link() { return &_link; }

    friend Debug & operator<<(Debug & db, const Buffer & b) 
    {
        db << "{lk=" << b._lock << ",bl=" << b._data << "}";
        return db;
    }

private:
    volatile bool _lock;
    T _data;
    Element _link;
};


__END_UTIL
 
#endif
