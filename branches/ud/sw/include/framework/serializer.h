#ifndef __serializer_sw_h
#define __serializer_sw_h

template<unsigned int BUF_SIZE>
class Serializer
{
protected:
    typedef unsigned long pkt;

public:
    Serializer(): _begin(_buf) {}

    pkt * get_pkt_buf() { return _buf; }

    void serialize() {}

    template<typename T>
    void serialize(const T & a) {
        Serialization<pkt>::serialize(_begin, a);
        _begin = &_begin[data_to_pkt<T>::Result];
    }

    template<typename T, typename ... Tn>
    void serialize(const T & a, const Tn & ... an) {
        Serialization<pkt>::serialize(_begin, a);
        _begin = &_begin[data_to_pkt<T>::Result];
        serialize(an...);
    }

    void deserialize() {}

    template<typename T>
    void deserialize(T & a) {
        Serialization<pkt>::deserialize(_begin, a);
        _begin = &_begin[data_to_pkt<T>::Result];
    }

    template<typename T, typename ... Tn>
    void deserialize(T & a, const Tn & ... an) {
        Serialization<pkt>::deserialize(_begin, a);
        _begin = &_begin[data_to_pkt<T>::Result];
        deserialize(an...);
    }

    void reset() { _begin = _buf; }

private:
    pkt * _begin;
    pkt _buf[DIV_ROUNDUP<BUF_SIZE, sizeof(pkt)>::Result];
};

#endif
