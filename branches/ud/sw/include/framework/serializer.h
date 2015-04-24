#ifndef __serializer_sw_h
#define __serializer_sw_h

#include "../../../unified/framework/serializer.h"

__BEGIN_SYS

class Serializer: public Serializer_Common
{
public:
    static void serialize(Packet * buf, int index) {}

    template<typename T>
    static void serialize(Packet * buf, int index, const T && a) {
        pack(&buf[index], a);
    }

    template<typename T, typename ... Tn>
    static void serialize(Packet * buf, int index, const T && a, const Tn & ... an) {
        pack(&buf[index], a);
        serialize(buf, index + npkt1<T>::Result, an ...);
    }

    static void deserialize(Packet * buf, int index) {}

    template<typename T>
    static void deserialize(Packet * buf, int index, T && a) {
        unpack(&buf[index], a);
    }

    template<typename T, typename ... Tn>
    static void deserialize(Packet * buf, int index, T && a, Tn & ... an) {
        unpack(&buf[index], a);
        deserialize(buf, index + npkt1<T>::Result, an ...);
    }
};

__END_SYS

#endif
