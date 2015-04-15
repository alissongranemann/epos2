#ifndef __serializer_hw_h
#define __serializer_hw_h

template<unsigned int BUF_SIZE>
class Serializer
{
protected:
    typedef unsigned long pkt;

public:
    Serializer(): _begin(_buf) {}

    pkt * get_pkt_buf() { return _buf; }

    template<typename T0>
    void serialize(T0 const &a0) {
        Serialization<pkt>::serialize(_begin, a0);
        _begin = &_begin[data_to_pkt<T0>::Result];
    }

    template<typename T0>
    void deserialize(T0 &a0) {
        Serialization<pkt>::deserialize(_begin, a0);
        _begin = &_begin[data_to_pkt<T0>::Result];
    }

    void reset() { _begin = _buf; }

private:
    pkt * _begin;
    pkt _buf[DIV_ROUNDUP<BUF_SIZE, sizeof(pkt)>::Result];
};

#endif
