#ifndef __serializer_hw_h
#define __serializer_hw_h

template<unsigned int BUF_SIZE>
class Serializer
{
public:
    typedef unsigned long Packet;

public:
    Serializer(): _begin(_buf) {}

    Packet * get_pkt_buf() { return _buf; }

    // reset() must be called prior to serialize() and deserialize()
    template<typename T0>
    void serialize(T0 const & a0) {
        Serialization<Packet>::serialize(_begin, a0);
        _begin = &_begin[data_to_pkt<T0>::Result];
    }

    template<typename T0>
    void deserialize(T0 & a0) {
        Serialization<Packet>::deserialize(_begin, a0);
        _begin = &_begin[data_to_pkt<T0>::Result];
    }

    void reset() { _begin = _buf; }

private:
    Packet * _begin;
    Packet _buf[DIV_ROUNDUP<BUF_SIZE, sizeof(Packet)>::Result];
};

#endif
