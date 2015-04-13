// EPOS Adder Abstraction Declarations

#ifndef __adder_unified_h
#define __adder_unified_h

__BEGIN_SYS

class Adder
{
public:
    enum {
        COMPONENT = 0x10,
        ADDER_ADD = COMPONENT,
        GET_INST_ID,
        SET_INST_ID,
        UNDEFINED = -1
    };

public:
    Adder(Channel_t &rx_ch, Channel_t &tx_ch, unsigned int inst_id) {}
    ~Adder() {}

    int add(int a, int b) { return (a + b); }
};

__END_SYS

#endif
