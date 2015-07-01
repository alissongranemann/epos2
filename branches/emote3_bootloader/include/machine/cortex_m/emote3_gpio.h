#ifndef __emote3_gpio_h_
#define __emote3_gpio_h_

__BEGIN_SYS

class GPIO : private Cortex_M_Model
{
    friend class eMote3;
public:
    /* TODO: GPIO interrupts are not working yet */
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

    typedef void (*GPIO_Handler)(GPIO * pin);

    GPIO(char port_letter, int pin_number, Direction dir) : _pin_bit(1 << pin_number)
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

    // TODO: set up one interrupt per pin

    // Called automatically by the handler
    void clear_interrupt() { reg(ICR) &= ~_pin_bit; }

    // Disable interrupts for this pin
    void disable_interrupt();

    // Enable interrupts for this pin
    void enable_interrupt(Edge e, GPIO_Handler h);

    void enable_interrupt(Level l, Handler * h)
    {
        //TODO
    }


private:
    GPIO_Handler _user_handler;

    static GPIO * requester_pin[4];
    static void gpio_int_handler(const unsigned int & irq_number)
    {
        kout << irq_number << endl;
        requester_pin[irq_number]->clear_interrupt();
        (*(requester_pin[irq_number]->_user_handler))(requester_pin[irq_number]);
    }

    volatile Reg32 * _data;
    volatile Reg32 & (*reg)(unsigned int);
    int _pin_bit;
    int _irq;
    int _over;
};

__END_SYS

#endif
