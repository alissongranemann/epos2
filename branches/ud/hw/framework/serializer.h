#ifndef __serializer_hw_h
#define __serializer_hw_h

#include "catapult.h"

// unsigned long pkts
// Specializations for int
template<> template<>
void Serialization<unsigned long >::serialize(unsigned long pkts[1], int &arg) {
    pkts[0] = arg;
}

template<> template<>
void Serialization<unsigned long >::deserialize(unsigned long pkts[1], int &arg) {
    arg = pkts[0];
}

// Specializations for unsigned int
template<> template<>
void Serialization<unsigned long >::serialize(unsigned long pkts[1], unsigned int &arg) {
    pkts[0] = arg;
}

template<> template<>
void Serialization<unsigned long >::deserialize(unsigned long pkts[1], unsigned int &arg) {
    arg = pkts[0];
}

// unsigned int pkts
// Specializations for int
template<> template<>
void Serialization<unsigned int >::serialize(unsigned int pkts[1], int &arg) {
    pkts[0] = arg;
}

template<> template<>
void Serialization<unsigned int >::deserialize(unsigned int pkts[1], int &arg) {
    arg = pkts[0];
}

// Specializations for unsigned int
template<> template<>
void Serialization<unsigned int >::serialize(unsigned int pkts[1], unsigned int &arg) {
    pkts[0] = arg;
}

template<> template<>
void Serialization<unsigned int >::deserialize(unsigned int pkts[1], unsigned int &arg) {
    arg = pkts[0];
}

template<unsigned int BUF_SIZE>
class Serializer
{
protected:
    typedef unsigned long pkt;

public:
    Serializer(): _begin(&_buf[0]), _pkt_cnt(0) {}

    pkt * get_pkt_buf() { return &_buf[0]; }

    void set_pkt_cnt(int cnt) { _pkt_cnt = cnt; }

    template<typename T0>
    void serialize(T0 &a0) {
        Serialization<pkt>::serialize(_begin, a0);
        _begin = &_begin[data_to_pkt<T0>::Result];
        _pkt_cnt += data_to_pkt<T0>::Result;
    }

    template<typename T0>
    void deserialize(T0 &a0) {
        Serialization<pkt>::deserialize(_begin, a0);
        _begin = &_begin[data_to_pkt<T0>::Result];
        _pkt_cnt -= data_to_pkt<T0>::Result;
    }

    void reset() { _begin = &_buf[0]; }

private:
    pkt * _begin;
    pkt _buf[DIV_ROUNDUP<BUF_SIZE, sizeof(pkt)>::Result];
    unsigned int _pkt_cnt;
};

#endif
