#ifndef __serializer_hw_h
#define __serializer_hw_h

class Serializer
{
public:
    typedef unsigned long Packet;

public:
    template<typename T>
    static void serialize(Packet * buf, int index, const T & a) {
        Serialization<Packet>::serialize(&buf[index], a);
    }

    template<typename T>
    static void deserialize(Packet * buf, int index, T & a) {
        Serialization<Packet>::deserialize(&buf[index], a);
    }
};

#endif
