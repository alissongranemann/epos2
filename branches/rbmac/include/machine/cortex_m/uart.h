// EPOS Cortex_M UART Mediator Declarations

#include __MODEL_H

#ifndef __cortex_m_uart_h__
#define __cortex_m_uart_h__

#include <cpu.h>
#include <uart.h>

__BEGIN_SYS

class Cortex_M_UART: private UART_Common, private Cortex_M_Model_UART
{
private:
    typedef Cortex_M_Model_UART Engine;

    static const unsigned int BAUD_RATE = Traits<Cortex_M_UART>::DEF_BAUD_RATE;
    static const unsigned int DATA_BITS = Traits<Cortex_M_UART>::DEF_DATA_BITS;
    static const unsigned int PARITY = Traits<Cortex_M_UART>::DEF_PARITY;
    static const unsigned int STOP_BITS = Traits<Cortex_M_UART>::DEF_STOP_BITS;

public:
    Cortex_M_UART(unsigned int baud_rate = BAUD_RATE, unsigned int data_bits = DATA_BITS,
                unsigned int parity = PARITY, unsigned int stop_bits = STOP_BITS, unsigned int unit = 0):
        Engine(unit, baud_rate, data_bits, parity, stop_bits) {}

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        Engine::config(baud_rate, data_bits, parity, stop_bits);
    }
    void config(unsigned int * baud_rate, unsigned int * data_bits, unsigned int * parity, unsigned int * stop_bits) {
        Engine::config(*baud_rate, *data_bits, *parity, *stop_bits);
    }

    volatile bool has_data() { return rxd_ok(); }
    char get() { while(!rxd_ok()); return rxd(); }
    void put(char c) { while(!txd_ok()); txd(c); }

    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        Engine::int_enable(receive, send, line, modem);
    }
    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        Engine::int_disable(receive, send, line, modem);
    }

    void reset() { Engine::config(BAUD_RATE, DATA_BITS, PARITY, STOP_BITS); }
    void loopback(bool flag) { Engine::loopback(flag); }
};

__END_SYS

#endif
