#ifndef __cortex_m_gpio_h_
#define __cortex_m_gpio_h_

#include <machine.h>
#include <utility/handler.h>

__BEGIN_SYS

class Cortex_M_GPIO : private Cortex_M_Model
{
public:
    enum Level {
        HIGH,
        LOW,
    };
    enum Edge {
        RISING_EDGE,
        FALLING_EDGE,
        BOTH_EDGES,
    };
    enum Direction {
        INPUT = 0,
        OUTPUT,
    };

    Cortex_M_GPIO(char port_letter, int pin_number, Direction dir) :
        _pin_bit(1 << pin_number),
        _pin_number(pin_number)
    {
        if(port_letter >= 'A' and port_letter <= 'Z') {
            _port = port_letter - 'A';
        } else if(port_letter >= 'a' and port_letter <= 'z') {
            _port = port_letter - 'a';
        } else {
            _port = GPIO_PORTS;
        }
        assert(_port < GPIO_PORTS);

        gpio(_port, AFSEL) &= ~_pin_bit; // Set pin as software controlled

        if(dir == OUTPUT) {
            output();
        } else if (dir == INPUT) {
            input();
        }

        _data = &gpio(_port, _pin_bit << 2);

        clear_interrupt();
    }

    void set(bool value = true) { *_data = 0xff*value; }
    void clear() { set(false); }
    volatile bool read() { return *_data; }
    volatile bool get() { return read(); }

    void output() { gpio(_port, DIR) |= _pin_bit; }
    void input() { gpio(_port, DIR) &= ~_pin_bit; }

    void pull_up() { gpio_pull_up(_port, _pin_number); }
    void pull_down() { gpio_pull_down(_port, _pin_number); }

    // Disable interrupts for this pin
    void disable_interrupt() { gpio(_port, IM) &= ~_pin_bit; }

    // Enable interrupts for this pin
    void enable_interrupt(Edge e, Handler * h, bool power_up = false, Edge power_up_edge = RISING_EDGE);

    //void enable_interrupt(Level l, Handler * h); // TODO

private:
    Handler * _user_handler;

    static Cortex_M_GPIO * requester_pin[GPIO_PORTS][8];
    static void gpio_int_handler(const unsigned int & int_number);
    
    // Cleared automatically by the handler
    void clear_interrupt() {
        gpio(_port, ICR) = _pin_bit; 
        gpio(_port, IRQ_DETECT_ACK) &= ~(_pin_bit << (8*_port));
    }

    unsigned int irq() { return _port; }

    volatile Reg32 * _data;
    unsigned int _pin_bit;
    unsigned int _pin_number;
    unsigned int _port;
};

__END_SYS

#endif
