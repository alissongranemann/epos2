#ifndef __rtscnoc_h
#define __rtscnoc_h

__BEGIN_SYS

class RTSNoC
{
public:
    // Packet types
    enum {
        CALL = 0,
        RESP,
        CALL_DATA,
        RESP_DATA,
        ERROR
    };

    // Locals
    enum {
        NN = 0,
        NE,
        EE,
        SE,
        SS,
        SW,
        WW,
        NW,
    };

    struct Header {
        unsigned char type;
        unsigned char inst_id;
        unsigned char type_id;
    };

    // Source address for received messages, destination address for transmitted
    // messages
    class Address {
    public:
        Address() {}
        Address(unsigned char x, unsigned char y, unsigned char local): _x(x),
            _y(y), _local(local) {}

        unsigned char x() { return _x; }
        unsigned char y() { return _y; }
        unsigned char local() { return _local; }

    private:
        unsigned char _x;
        unsigned char _y;
        unsigned char _local;
    };

    struct Packet {
        unsigned long payload;
        Header header;
        Address addr;
    };
};

__END_SYS

#endif
