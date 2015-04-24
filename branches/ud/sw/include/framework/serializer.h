#ifndef __serializer_sw_h
#define __serializer_sw_h

class Serializer
{
public:
    typedef unsigned long Packet;

public:
    static void serialize(Packet * buf, int index) {}

    template<typename T>
    static void serialize(Packet * buf, int index, const T && a) {
        Serialization<Packet>::serialize(&buf[index], a);
    }

    template<typename T, typename ... Tn>
    static void serialize(Packet * buf, int index, const T && a, const Tn & ... an) {
        Serialization<Packet>::serialize(&buf[index], a);
        serialize(buf, index + type_to_npkt_1<T>::Result, an ...);
    }

    static void deserialize(Packet * buf, int index) {}

    template<typename T>
    static void deserialize(Packet * buf, int index, T && a) {
        Serialization<Packet>::deserialize(&buf[index], a);
    }

    template<typename T, typename ... Tn>
    static void deserialize(Packet * buf, int index, T && a, Tn & ... an) {
        Serialization<Packet>::deserialize(&buf[index], a);
        deserialize(buf, index + type_to_npkt_1<T>::Result, an ...);
    }
};

#endif
