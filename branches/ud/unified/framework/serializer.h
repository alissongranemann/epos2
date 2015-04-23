#ifndef __serializer_h
#define __serializer_h

#include <system/config.h>

__BEGIN_SYS

template<typename Pkt_T>
class Serialization
{
public:
    template<typename Arg_T>
    static void serialize(Pkt_T * pkts, Arg_T &arg);

    template<typename Arg_T>
    static void deserialize(Pkt_T * pkts, Arg_T &arg);
};

// unsigned long is Serializer::pkt type
template<typename T0>
struct type_to_npkt_1 {
    enum { Result = DIV_ROUNDUP<sizeof(T0), sizeof(unsigned long)>::Result };
};

template<typename T0, typename T1>
struct type_to_npkt_2 {
    enum { Result = type_to_npkt_1<T0>::Result + type_to_npkt_1<T1>::Result };
};

template<typename T0, typename T1, typename T2>
struct type_to_npkt_3 {
    enum {
        Result = type_to_npkt_1<T0>::Result + type_to_npkt_1<T1>::Result +
            type_to_npkt_1<T2>::Result
    };
};

template<typename T0, typename T1, typename T2, typename T3>
struct type_to_npkt_4 {
    enum {
        Result = type_to_npkt_1<T0>::Result + type_to_npkt_1<T1>::Result +
            type_to_npkt_1<T2>::Result + type_to_npkt_1<T3>::Result
    };
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
struct type_to_npkt_8 {
    enum {
        Result = type_to_npkt_1<T0>::Result + type_to_npkt_1<T1>::Result +
            type_to_npkt_1<T2>::Result + type_to_npkt_1<T3>::Result +
            type_to_npkt_1<T4>::Result + type_to_npkt_1<T5>::Result +
            type_to_npkt_1<T6>::Result + type_to_npkt_1<T7>::Result
    };
};

// unsigned long pkts
// Specializations for int
template<> template<>
void Serialization<unsigned long>::serialize(unsigned long pkts[1], const int &arg) {
    pkts[0] = arg;
}

template<> template<>
void Serialization<unsigned long>::deserialize(unsigned long pkts[1], int &arg) {
    arg = pkts[0];
}

// Specializations for unsigned int
template<> template<>
void Serialization<unsigned long>::serialize(unsigned long pkts[1], const unsigned int &arg) {
    pkts[0] = arg;
}

template<> template<>
void Serialization<unsigned long>::deserialize(unsigned long pkts[1], unsigned int &arg) {
    arg = pkts[0];
}

// TODO: Don't use ifdefs!
#ifdef HIGH_LEVEL_SYNTHESIS
#include "../../hw/framework/serializer.h"
#else
#include "../../sw/include/framework/serializer.h"
#endif

__END_SYS

#endif
