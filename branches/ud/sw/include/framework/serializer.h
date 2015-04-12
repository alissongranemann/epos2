#ifndef __serializer_sw_h
#define __serializer_sw_h

// Specializations for int
template<> template<>
inline void Serialization<unsigned int>::serialize(unsigned int * pkt, const int &arg) {
    *pkt = arg;
}
template<> template<>
inline void Serialization<unsigned int>::deserialize(unsigned int * pkt, int &arg) {
    arg = *pkt;
}

// Specializations for unsigned int
template<> template<>
inline void Serialization<unsigned int>::serialize(unsigned int * pkt, const unsigned int &arg) {
    *pkt = arg;
}
template <> template<>
inline void Serialization<unsigned int>::deserialize(unsigned int * pkt, unsigned int &arg) {
    arg = *pkt;
}

// Specializations for unsigned char
template<> template<>
inline void Serialization<unsigned int>::serialize(unsigned int * pkt, const unsigned char &arg) {
    *pkt = arg;
}
template<> template<>
inline void Serialization<unsigned int>::deserialize(unsigned int * pkt, unsigned char &arg) {
    arg = *pkt;
}

template<unsigned int BUF_SIZE>
class Serializer
{
public:
    //typedef IF<Traits<Sys>::serdes_pkt_size==8, unsigned long long int,
            //IF<Traits<Sys>::serdes_pkt_size==4, unsigned int,
            //IF<Traits<Sys>::serdes_pkt_size==2, unsigned short,
            //IF<Traits<Sys>::serdes_pkt_size==1,  unsigned char, void>::Result>::Result>::Result>::Result pkt_type;

    typedef unsigned int pkt_type;

private:
    pkt_type * _begin;
    unsigned int _data_cnt;
    pkt_type _buffer[DIV_ROUNDUP<BUF_SIZE, 4>::Result];

public:
    Serializer(): _begin(&_buffer[0]), _data_cnt(0) {}

public:
    pkt_type * get_pkt_buffer() { return _buffer; }

    int get_pkt_cnt() { return _data_cnt; }
    void set_pkt_cnt(int d) { _data_cnt = d; }

    void reset() { _begin = &_buffer[0]; }

    void serialize() {}

    template<typename T>
    void serialize(const T & a) {
        Serialization<pkt_type>::serialize(_begin, a);
        _begin = &_begin[data_to_pkt<T>::Result];
        _data_cnt += data_to_pkt<T>::Result;
    }

    template<typename T, typename ... Tn>
    void serialize(const T & a, const Tn & ... an) {
        Serialization<pkt_type>::serialize(_begin, a);
        _begin = &_begin[data_to_pkt<T>::Result];
        _data_cnt += data_to_pkt<T>::Result;
        serialize(an...);
    }

    void deserialize() {}

    template<typename T>
    void deserialize(T & a) {
        Serialization<pkt_type>::deserialize(_begin, a);
        _begin = &_begin[data_to_pkt<T>::Result];
        _data_cnt -= data_to_pkt<T>::Result;
    }

    template<typename T, typename ... Tn>
    void deserialize(T & a, const Tn & ... an) {
        Serialization<pkt_type>::deserialize(_begin, a);
        _begin = &_begin[data_to_pkt<T>::Result];
        _data_cnt -= data_to_pkt<T>::Result;
        deserialize(an...);
    }
};

#endif
