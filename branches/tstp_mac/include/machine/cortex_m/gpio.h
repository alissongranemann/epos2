#ifndef __cortex_m_gpio_h_
#define __cortex_m_gpio_h_

#include <machine.h>

__BEGIN_SYS

class Cortex_M_GPIO : private Cortex_M_Model
{
//    friend class eMote3;
public:
    enum Level
    {
        HIGH,
        LOW,
    };
    enum Edge
    {
        RISING_EDGE,
        FALLING_EDGE,
        BOTH_EDGES,
    };
    enum Direction
    {
        INPUT = 0,
        OUTPUT,
    };

    static Cortex_M_GPIO _radio_sending;
    static Cortex_M_GPIO _radio_receiving;

    typedef void (*GPIO_Handler)(Cortex_M_GPIO * pin);

    Cortex_M_GPIO(char port_letter, int pin_number, Direction dir) :
        _pin_bit(1 << pin_number),
        _pin_number(pin_number)
    {
        switch(port_letter)
        {
            default:
            case 'a': case 'A': reg = &gpioa; _irq = 0; break;
            case 'b': case 'B': reg = &gpiob; _irq = 1; break;
            case 'c': case 'C': reg = &gpioc; _irq = 2; break;
            case 'd': case 'D': reg = &gpiod; _irq = 3; break;
        }

        // Calculate the offset for the GPIO's IOC_Pxx_OVER
        _over = PA0_OVER + 0x20*_irq + 0x4*pin_number;

        reg(AFSEL) &= ~_pin_bit; // Set pin as software controlled
        if(dir == OUTPUT)
            output();
        else if (dir == INPUT)
            input();

        _data = &reg(_pin_bit << 2);

        clear_interrupt();
    }

    void set(bool value = true) { *_data = 0xff*value; }
    void clear() { set(false); }
    volatile bool read() { return *_data; }
    volatile bool get() { return read(); }

    void output() { reg(DIR) |= _pin_bit; }
    void input() { reg(DIR) &= ~_pin_bit; }

    void pull_up() { ioc(_over) = PUE; }
    void pull_down() { ioc(_over) = PDE; }

    // Called automatically by the handler
    void clear_interrupt();

    // Disable interrupts for this pin
    void disable_interrupt();

    // Enable interrupts for this pin
    void enable_interrupt(Edge e, GPIO_Handler h);

    //void enable_interrupt(Level l, GPIO_Handler * h); // TODO

private:
    GPIO_Handler _user_handler;

    static Cortex_M_GPIO * requester_pin[4][8];
    static void gpio_int_handler(const unsigned int & int_number);

    volatile Reg32 * _data;
    volatile Reg32 & (*reg)(unsigned int);
    int _pin_bit;
    unsigned int _pin_number;
    int _irq;
    int _over;
};

__END_SYS

#endif
