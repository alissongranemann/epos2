// EPOS Cortex-M GPIO Mediator Implementation

#include <machine.h>
#include <ic.h>
#include <machine/cortex_m/gpio.h>

__BEGIN_SYS

// Class attributes
GPIO * GPIO::_devices[GPIO_PORTS][8];

// Class methods
void GPIO::int_handler(const IC::Interrupt_Id & i)
{
    unsigned int port = IC::int2irq(i); // TODO: confirm that this is a firm mapping (i.e. PORT[A][0] triggers IRQ0, PORT[A][1] triggers IRQ1, PORT[B][0] triggers IRQ8

    for(unsigned int i = 0; i < 8; ++i) {
        const bool regular_interrupt = gpio(port, MIS) & (1 << i);
        const bool power_up_interrupt = gpio(port, IRQ_DETECT_ACK) & ((1 << i) << (8 * port));
        if(regular_interrupt || power_up_interrupt) {
            GPIO * dev = _devices[port][i];
            if(dev && dev->_handler)
                dev->_handler(i);
        }
    }

    // Clear regular interrupts even if no handler is available
    gpio(port, ICR) = -1;

    // Clear power-up interrupts even if no handler is available
    // There is something weird going on here.
    // The manual says: "There is a self-clearing function to this register that generates a
    // reset pulse to clear any interrupt which has its corresponding bit set to 1."
    // But this is not happening! 
    // Also, clearing only the bit that is set or replacing the statement below with
    // regs[irq_number](IRQ_DETECT_ACK) = 0;
    // do not work!
    gpio(port, IRQ_DETECT_ACK) &= -1;
}

void GPIO::int_enable(const Edge & edge, bool power_up, const Edge & power_up_edge)
{
    IC::disable(_port);
//    IC::unpend(irq());
    int_disable();
    clear_interrupt();
    IC::int_vector(IC::irq2int(_port), GPIO::int_handler);
    gpio(_port, IS) &= ~_pin_bit; // Set interrupt to edge-triggered
    if(edge == BOTH)
        gpio(_port, IBE) |= _pin_bit; // Interrupt on both edges
    else {
        gpio(_port, IBE) &= ~_pin_bit; // Interrupt on single edge, defined by IEV
        if(edge == RISING)
            gpio(_port, IEV) |= _pin_bit; // Interrupt on rising edge
        else if(edge == FALLING)
            gpio(_port, IEV) &= ~_pin_bit; // Interrupt on falling edge
    }

    switch(edge) {
    case RISING:
        gpio(_port, IBE) &= ~_pin_bit; // Interrupt on single edge, defined by IEV
        gpio(_port, IEV) |= _pin_bit;
        break;
    case FALLING:
        gpio(_port, IBE) &= ~_pin_bit; // Interrupt on single edge, defined by IEV
        gpio(_port, IEV) &= ~_pin_bit;
        break;
    case BOTH:
        gpio(_port, IBE) |= _pin_bit;
        break;
    }

    clear_interrupt();
    gpio(_port, IM) |= _pin_bit; // Enable interrupts for this pin 

    if(supports_power_up && power_up) {
        switch(power_up_edge) {
        case FALLING:
            gpio(_port, P_EDGE_CTRL) |= (_pin_bit << (8 * _port));
            break;
        case RISING:
            gpio(_port, P_EDGE_CTRL) &= ~(_pin_bit << (8 * _port));
            break;
        case BOTH:
            db<GPIO>(ERR) << "GPIO::int_enable: power up interrupt must choose either rising or falling edge!" << endl;
            break;
        }
        gpio(_port, PI_IEN) |= (_pin_bit << (8 * _port));
    }

    IC::enable(_port);
}

__END_SYS