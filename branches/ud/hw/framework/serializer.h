#ifndef __serializer_hw_h
#define __serializer_hw_h

#include <framework/serializer.h>

// TODO: Keep only one Serializer version when Catapult starts supporting
// variadic templates

__BEGIN_SYS

class Serializer: public Serializer_Common
{
public:
    template<typename T>
    static void serialize(Packet * buf, int index, const T & a) {
        pack(&buf[index], a);
    }

    template<typename T>
    static void deserialize(Packet * buf, int index, T & a) {
        unpack(&buf[index], a);
    }
};

__END_SYS

#endif
