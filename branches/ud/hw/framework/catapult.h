#ifndef __catapult_h
#define __catapult_h

#include <ac_channel.h>

namespace Catapult {

struct RMI_Msg
{
private:
    typedef struct{
        unsigned char type;
        unsigned char inst_id;
        unsigned char type_id;
    } Header;

    typedef struct {
        unsigned char x;
        unsigned char y;
        unsigned char local;
    } Address;

public:
    // TODO: Payload size should be Traits<System>::pkt_size
    unsigned long payload;
    Header header;
    // Source address for received messages, destination address for transmitted
    // messages
    Address addr;
};

// The message's members order above will yield the following packet
// organization:
// 79...72 71...64 63...56 55...48 47...49 39...32 31...0
// local   y       x       type_id inst_id type    payload
typedef ac_channel<RMI_Msg> Channel_t;

};

#endif
