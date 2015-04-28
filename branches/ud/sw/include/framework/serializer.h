#ifndef __serializer_sw_h
#define __serializer_sw_h

#include "../../../unified/framework/serializer.h"

__BEGIN_SYS

class Serializer: public Serializer_Common
{
public:
    static void serialize(Buffer * buf, int index) {}

    template<typename ... Tn>
    struct NPKT
    { static const unsigned int  Result = 0; };

    template<typename T1, typename ... Tn>
    struct NPKT<T1, Tn ...>
    { static const unsigned int Result = NPKT1<T1>::Result + SIZEOF<Tn ...>::Result ; };

    template<typename T>
    static void serialize(Buffer * buf, int index, const T && a) {
        pack(&buf[index], a);
    }

    template<typename T, typename ... Tn>
    static void serialize(Buffer * buf, int index, const T && a, const Tn & ... an) {
        pack(&buf[index], a);
        serialize(buf, index + NPKT1<T>::Result, an ...);
    }

    static void deserialize(Buffer * buf, int index) {}

    template<typename T>
    static void deserialize(Buffer * buf, int index, T && a) {
        unpack(&buf[index], a);
    }

    template<typename T, typename ... Tn>
    static void deserialize(Buffer * buf, int index, T && a, Tn & ... an) {
        unpack(&buf[index], a);
        deserialize(buf, index + NPKT1<T>::Result, an ...);
    }
};

__END_SYS

#endif
