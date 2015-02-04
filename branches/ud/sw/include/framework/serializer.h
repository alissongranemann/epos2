/*
 * serializer.h
 *
 *  Created on: Jan 22, 2013
 *      Author: tiago
 */

#ifndef SERIALIZER_EPOS_SOC_SW_H_
#define SERIALIZER_EPOS_SOC_SW_H_

#include "../../../unified/framework/serializer_common.h"

//Specialization for pkt_type 'unsigned int'
namespace Implementation { namespace serializer_imp {

//Specializations for unsigned int
template<> template<>
inline void Serialization<unsigned int>::serialize(unsigned int pkts[1], unsigned int &arg){
    pkts[0] = arg;
}
template<> template<>
inline void Serialization<unsigned int>::deserialize(unsigned int pkts[1], unsigned int &arg){
    arg = pkts[0];
}

//Specializations for unsigned char
template<> template<>
inline void Serialization<unsigned int>::serialize(unsigned int pkts[1], unsigned char &arg){
    pkts[0] = arg;
}
template<> template<>
inline void Serialization<unsigned int>::deserialize(unsigned int pkts[1], unsigned char &arg){
    arg = pkts[0];
}

//Specializations for unsigned int
template<> template<>
inline void Serialization<unsigned int>::serialize(unsigned int pkts[1], int &arg){
    pkts[0] = arg;
}
template<> template<>
inline void Serialization<unsigned int>::deserialize(unsigned int pkts[1], int &arg){
    arg = pkts[0];
}

//Specializations for safe_pkt
template<> template<>
inline void Serialization<unsigned int>::serialize(unsigned int pkts[sizeof(safe_pkt_t)/4], safe_pkt_t &arg){
    for (unsigned int i = 0; i < sizeof(safe_pkt_t)/4; ++i) {
        pkts[i] = 0;
        pkts[i] |= (unsigned int)arg.data[(i*4)+0];
        pkts[i] |= ((unsigned int)arg.data[(i*4)+1] << 8);
        pkts[i] |= ((unsigned int)arg.data[(i*4)+2] << 16);
        pkts[i] |= ((unsigned int)arg.data[(i*4)+3] << 24);
    }
}
template<> template<>
inline void Serialization<unsigned int>::deserialize(unsigned int pkts[sizeof(safe_pkt_t)/4], safe_pkt_t &arg){
    for (unsigned int i = 0; i < sizeof(safe_pkt_t)/4; ++i) {
        arg.data[(i*4)+0] = pkts[i] & 0x000000FF;
        arg.data[(i*4)+1] = (pkts[i] >> 8) & 0x000000FF;
        arg.data[(i*4)+2] = (pkts[i] >> 16) & 0x000000FF;
        arg.data[(i*4)+3] = (pkts[i] >> 24) & 0x000000FF;
    }
}

//Specializations for decoded_pkt
template<> template<>
inline void Serialization<unsigned int>::serialize(unsigned int pkts[sizeof(decoded_pkt_t)/4], decoded_pkt_t &arg){
    for (unsigned int i = 0; i < sizeof(decoded_pkt_t)/4; ++i) {
        pkts[i] = 0;
        pkts[i] |= (unsigned int)arg.data[(i*2)];
        pkts[i] |= ((unsigned int)arg.data[(i*2)+1] << 16);
    }
}
template<> template<>
inline void Serialization<unsigned int>::deserialize(unsigned int pkts[sizeof(decoded_pkt_t)/4], decoded_pkt_t &arg){
    for (unsigned int i = 0; i < sizeof(decoded_pkt_t)/4; ++i) {
        arg.data[(i*2)+0] = pkts[i] & 0x000000FF;
        arg.data[(i*2)+1] = (pkts[i] >> 16) & 0x000000FF;
    }
}

};};


namespace Implementation { namespace serializer_imp {

template<unsigned int BUFFER_SIZE>
class Serializer<BUFFER_SIZE,Configurations::EPOS_SOC_Catapult,false> {
public:
    typedef IF<Traits<Sys>::serdes_pkt_size==8, unsigned long long int,
            IF<Traits<Sys>::serdes_pkt_size==4, unsigned int,
            IF<Traits<Sys>::serdes_pkt_size==2, unsigned short,
            IF<Traits<Sys>::serdes_pkt_size==1,  unsigned char, void>::Result>::Result>::Result>::Result pkt_type;

private:
    pkt_type* _begin;
    unsigned int _data_cnt;
    pkt_type _buffer[DIV_ROUNDUP<BUFFER_SIZE,Traits<Sys>::serdes_pkt_size>::Result];

public:
    Serializer() :_begin(&_buffer[0]), _data_cnt(0){
    }

public:

    pkt_type* get_pkt_buffer(){
        return _buffer;
    }

    int get_pkt_cnt(){return _data_cnt;}
    void set_pkt_cnt(int d){_data_cnt = d;}

    void reset(){_begin = &_buffer[0];}

    template<typename ARG0>
    void serialize(ARG0 &arg0){
        Serialization<pkt_type>::serialize(_begin,arg0);
        _begin = &_begin[data_to_pkt<ARG0>::Result];
        _data_cnt += data_to_pkt<ARG0>::Result;
    }

    template<typename ARG0>
    void deserialize(ARG0 &arg0){
        Serialization<pkt_type>::deserialize(_begin,arg0);
        _begin = &_begin[data_to_pkt<ARG0>::Result];
        _data_cnt -= data_to_pkt<ARG0>::Result;
    }

};

}; };



#endif /* SERIALIZER_H_ */
