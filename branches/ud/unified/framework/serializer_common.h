/*
 * serializer_common.h
 *
 *  Created on: Apr 30, 2013
 *      Author: tiago
 */
#ifndef SERIALIZER_COMMON_H_
#define SERIALIZER_COMMON_H_

#include "../traits.h"

namespace Implementation {

namespace serializer_imp {

template<unsigned int BUFFER_SIZE,
         typename Platform,
         bool hardware>
class Serializer;

template<typename Pkt_type>
class Serialization {
public:
    template<typename Arg_type>
    static void serialize(Pkt_type *pkts, Arg_type &arg);

    template<typename Arg_type>
    static void deserialize(Pkt_type *pkts, Arg_type &arg);
};

};

template<typename DATA>
struct data_to_pkt{
    enum {
        Result = DIV_ROUNDUP<sizeof(DATA),Traits<Sys>::serdes_pkt_size>::Result
    };
};

template<typename TYPE0>
struct type_to_npkt_1{
    enum{ Result = data_to_pkt<TYPE0>::Result};
};

template<typename TYPE0,typename TYPE1>
struct type_to_npkt_2{
    enum{ Result = data_to_pkt<TYPE0>::Result +
        data_to_pkt<TYPE1>::Result
    };
};

template<typename TYPE0,typename TYPE1,typename TYPE2>
struct type_to_npkt_3{
    enum{ Result = data_to_pkt<TYPE0>::Result +
        data_to_pkt<TYPE1>::Result +
        data_to_pkt<TYPE2>::Result
    };
};

template<typename TYPE0,typename TYPE1,typename TYPE2,typename TYPE3>
struct type_to_npkt_4{
    enum{ Result = data_to_pkt<TYPE0>::Result +
        data_to_pkt<TYPE1>::Result +
        data_to_pkt<TYPE2>::Result +
        data_to_pkt<TYPE3>::Result
    };
};

template<typename TYPE0,typename TYPE1,typename TYPE2,typename TYPE3,typename TYPE4,typename TYPE5,typename TYPE6,typename TYPE7>
struct type_to_npkt_8{
    enum{ Result = data_to_pkt<TYPE0>::Result +
        data_to_pkt<TYPE1>::Result +
        data_to_pkt<TYPE2>::Result +
        data_to_pkt<TYPE3>::Result +
        data_to_pkt<TYPE4>::Result +
        data_to_pkt<TYPE5>::Result +
        data_to_pkt<TYPE6>::Result +
        data_to_pkt<TYPE7>::Result
    };
};

};

#endif
