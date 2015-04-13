#ifndef __serializer_hw_h
#define __serializer_hw_h

#include "catapult.h"

// Specialization for pkt 'ac_int<32,false>'

// Specializations for unsigned int
template<> template<>
inline void Serialization<ac_int<32, false> >::serialize(ac_int<32, false> pkts[1], unsigned int &arg) {
    pkts[0] = arg;
}

template<> template<>
inline void Serialization<ac_int<32, false> >::deserialize(ac_int<32, false> pkts[1], unsigned int &arg) {
    arg = pkts[0];
}

// Specializations for unsigned char
template<> template<>
inline void Serialization<ac_int<32, false> >::serialize(ac_int<32, false> pkts[1], unsigned char &arg) {
    pkts[0] = arg;
}

template<> template<>
inline void Serialization<ac_int<32, false> >::deserialize(ac_int<32,false> pkts[1], unsigned char &arg) {
    arg = pkts[0];
}

// Specializations for int
template<> template<>
inline void Serialization<ac_int<32, false> >::serialize(ac_int<32, false> pkts[1], int &arg) {
    pkts[0] = arg;
}

template<> template<>
inline void Serialization<ac_int<32, false> >::deserialize(ac_int<32, false> pkts[1], int &arg) {
    arg = pkts[0];
}

// Specializations for unsigned long long
template<> template<>
inline void Serialization<ac_int<32, false> >::serialize(ac_int<32, false> pkts[sizeof(unsigned long long)/4], unsigned long long &arg) {
    ac_int<64, false> tmp = arg;

    pkts[0] = tmp.slc<32>(0);
    pkts[1] = tmp.slc<32>(32);
}

template<> template<>
inline void Serialization<ac_int<32, false> >::deserialize(ac_int<32, false> pkts[sizeof(unsigned long long)/4], unsigned long long &arg) {
    ac_int<64, false> tmp;

    tmp.set_slc(0, pkts[0]);
    tmp.set_slc(32, pkts[1]);

    arg = (unsigned long long)tmp;
}

template<unsigned int BUF_SIZE>
class Serializer
{
private:
    enum {
        PKT_N_BYTES = 4,
        PKT_N_BITS  = 8*PKT_N_BYTES
    };

protected:
    typedef ac_int<PKT_N_BITS, false> pkt;

public:
    Serializer(): _begin(&_buf[0]), _data_cnt(0) {}

    pkt * get_pkt_buf() { return &_buf[0]; }

    int get_pkt_cnt() { return _data_cnt; }
    void set_pkt_cnt(int d) { _data_cnt = d; }

    void reset() { _begin = &_buf[0]; }

    template<typename T0>
    void serialize(T0 &a0) {
        Serialization<pkt>::serialize(_begin, a0);
        _begin = &_begin[data_to_pkt<T0>::Result];
        _data_cnt += data_to_pkt<T0>::Result;
    }

    template<typename T0>
    void deserialize(T0 &a0) {
        Serialization<pkt>::deserialize(_begin, a0);
        _begin = &_begin[data_to_pkt<T0>::Result];
        _data_cnt -= data_to_pkt<T0>::Result;
    }

private:
    pkt * _begin;
    unsigned int _data_cnt;
    pkt _buf[DIV_ROUNDUP<BUF_SIZE, PKT_N_BYTES>::Result];
};

#endif
