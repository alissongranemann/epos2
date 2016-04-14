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
    struct Address {
        unsigned char _x;
        unsigned char _y;
        unsigned char _h;
    };

    // Packet has the following organization:
    // 79...72 71...64 63...56 55...48 47...40 39...32 31...0
    // local   y       x       type_id inst_id type    payload
    struct Packet {
        unsigned int payload;
        Header header;
        Address addr;
    };
};

__END_SYS

#endif
