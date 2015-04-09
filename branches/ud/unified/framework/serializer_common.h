#ifndef __serializer_common_h
#define __serializer_common_h

#include "../../sw/include/system/meta.h"

__BEGIN_SYS

template<unsigned int BUFFER_SIZE>
class Serializer;

template<typename Pkt_T>
class Serialization
{
public:
    template<typename Arg_T>
    static void serialize(Pkt_T * pkts, Arg_T &arg);

    template<typename Arg_T>
    static void deserialize(Pkt_T * pkts, Arg_T &arg);
};

template<int T1, int T2>
struct DIV_ROUNDUP {
    enum {
        _aux = T1/T2,
        _aux_mod = T1%T2,
        Result = IF_INT<(_aux_mod != 0),(_aux + 1),_aux>::Result
    };
};

template<typename DATA>
struct data_to_pkt {
    enum { Result = DIV_ROUNDUP<sizeof(DATA), 4>::Result };
};

template<typename TYPE0>
struct type_to_npkt_1 {
    enum { Result = data_to_pkt<TYPE0>::Result };
};

template<typename TYPE0, typename TYPE1>
struct type_to_npkt_2 {
    enum { Result = data_to_pkt<TYPE0>::Result + data_to_pkt<TYPE1>::Result };
};

template<typename TYPE0, typename TYPE1, typename TYPE2>
struct type_to_npkt_3 {
    enum {
        Result = data_to_pkt<TYPE0>::Result + data_to_pkt<TYPE1>::Result +
            data_to_pkt<TYPE2>::Result
    };
};

template<typename TYPE0, typename TYPE1, typename TYPE2, typename TYPE3>
struct type_to_npkt_4 {
    enum {
        Result = data_to_pkt<TYPE0>::Result + data_to_pkt<TYPE1>::Result +
            data_to_pkt<TYPE2>::Result + data_to_pkt<TYPE3>::Result
    };
};

template<typename TYPE0, typename TYPE1, typename TYPE2, typename TYPE3, typename TYPE4, typename TYPE5, typename TYPE6, typename TYPE7>
struct type_to_npkt_8 {
    enum {
        Result = data_to_pkt<TYPE0>::Result + data_to_pkt<TYPE1>::Result +
            data_to_pkt<TYPE2>::Result + data_to_pkt<TYPE3>::Result +
            data_to_pkt<TYPE4>::Result + data_to_pkt<TYPE5>::Result +
            data_to_pkt<TYPE6>::Result + data_to_pkt<TYPE7>::Result
    };
};

__END_SYS

#endif
