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
template<typename DATA>
struct data_to_pkt {
    enum { Result = DIV_ROUNDUP<sizeof(DATA), sizeof(unsigned long)>::Result };
};

template<typename T0>
struct type_to_npkt_1 {
    enum { Result = data_to_pkt<T0>::Result };
};

template<typename T0, typename T1>
struct type_to_npkt_2 {
    enum { Result = data_to_pkt<T0>::Result + data_to_pkt<T1>::Result };
};

template<typename T0, typename T1, typename T2>
struct type_to_npkt_3 {
    enum {
        Result = data_to_pkt<T0>::Result + data_to_pkt<T1>::Result +
            data_to_pkt<T2>::Result
    };
};

template<typename T0, typename T1, typename T2, typename T3>
struct type_to_npkt_4 {
    enum {
        Result = data_to_pkt<T0>::Result + data_to_pkt<T1>::Result +
            data_to_pkt<T2>::Result + data_to_pkt<T3>::Result
    };
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
struct type_to_npkt_8 {
    enum {
        Result = data_to_pkt<T0>::Result + data_to_pkt<T1>::Result +
            data_to_pkt<T2>::Result + data_to_pkt<T3>::Result +
            data_to_pkt<T4>::Result + data_to_pkt<T5>::Result +
            data_to_pkt<T6>::Result + data_to_pkt<T7>::Result
    };
};

// TODO: Don't use ifdefs!
#ifdef HIGH_LEVEL_SYNTHESIS
#include "../../hw/framework/serializer.h"
#else
#include "../../sw/include/framework/serializer.h"
#endif

__END_SYS

#endif
