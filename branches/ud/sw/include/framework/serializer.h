#ifndef __serializer_sw_h
#define __serializer_sw_h

template<unsigned int BUF_SIZE>
class Serializer
{
protected:
    typedef unsigned long Packet;

public:
    Serializer(): _begin(_buf) {}

    Packet * get_pkt_buf() { return _buf; }

    void serialize() {}

    template<typename T>
    void serialize(const T & a) {
        Serialization<Packet>::serialize(_begin, a);
        _begin = &_begin[type_to_npkt_1<T>::Result];
    }

    template<typename T, typename ... Tn>
    void serialize(const T & a, const Tn & ... an) {
        Serialization<Packet>::serialize(_begin, a);
        _begin = &_begin[type_to_npkt_1<T>::Result];
        serialize(an...);
    }

    void deserialize() {}

    template<typename T>
    void deserialize(T & a) {
        Serialization<Packet>::deserialize(_begin, a);
        _begin = &_begin[type_to_npkt_1<T>::Result];
    }

    template<typename T, typename ... Tn>
    void deserialize(T & a, const Tn & ... an) {
        Serialization<Packet>::deserialize(_begin, a);
        _begin = &_begin[type_to_npkt_1<T>::Result];
        deserialize(an...);
    }

    void reset() { _begin = _buf; }

private:
    Packet * _begin;
    Packet _buf[DIV_ROUNDUP<BUF_SIZE, sizeof(Packet)>::Result];
};

#endif
