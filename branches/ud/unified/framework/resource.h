#ifndef __resource_unified_h
#define __resource_unified_h

namespace Implementation {

class Address
{
public:
    enum {
        LOCAL_NN = 0,
        LOCAL_NE = 1,
        LOCAL_EE = 2,
        LOCAL_SE = 3,
        LOCAL_SS = 4,
        LOCAL_SW = 5,
        LOCAL_WW = 6,
        LOCAL_NW = 7,
    };

public:
    Address(): x(0), y(0), local(0){};

    Address(Address &o): x(o.x), y(o.y), local(o.local){};

    Address(unsigned int x, unsigned int y, unsigned int local): x(x), y(y),
        local(local){};

    bool operator==(const Address &a) const {
        return (x == a.x) && (y == a.y) && (local == a.local);
    }

    bool operator!=(const Address &a) const {
        return (x != a.x) || (y != a.y) || (local != a.local);
    }

    static unsigned int addr_to_id(Address const * addr,
            unsigned int net_x_size) {
        unsigned int id = addr->local;
        id |= addr->x << 3;
        id |= addr->y << (3 + net_x_size);

        return id;
    }

    static void id_to_addr(unsigned int id, Address * addr,
            unsigned int net_x_size) {
        unsigned int tmp = 2;

        for (unsigned int i = 1; i < net_x_size; ++i)
            tmp <<= 1;

        tmp -= 1;

        addr->y = id >> (3 + net_x_size);
        addr->x = (id >> 3) & tmp;
        addr->local = id & 0x7;
    }

public:
    unsigned int x;
    unsigned int y;
    unsigned int local;
};

}

#endif
