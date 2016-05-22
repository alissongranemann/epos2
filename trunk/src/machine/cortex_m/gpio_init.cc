#include <gpio.h>
#include <ic.h>

__BEGIN_SYS

GPIO * GPIO::requester_pin[Cortex_M_Model::GPIO_PORTS][8];

void GPIO::gpio_int_handler(const IC::Interrupt_Id & int_number)
{
    auto irq_number = IC::int2irq(int_number);

    for (auto i = 0u; i < 8; ++i) {
        const bool regular_interrupt = gpio(irq_number, MIS) & (1 << i);
        const bool power_up_interrupt = gpio(irq_number, IRQ_DETECT_ACK) & ((1 << i) << (8*irq_number));
        if (regular_interrupt or power_up_interrupt) {
            auto pin = requester_pin[irq_number][i];
            if(pin and pin->_user_handler) {
                (*(pin->_user_handler))();
            }
        }
    }
    // Clear regular interrupts even if no handler is available
    gpio(irq_number, ICR) = -1;

    // Clear power-up interrupts even if no handler is available
    // There is something weird going on here.
    // The manual says: "There is a self-clearing function to this register that generates a
    // reset pulse to clear any interrupt which has its corresponding bit set to 1."
    // But this is not happening! 
    // Also, clearing only the bit that is set or replacing the statement below with
    // regs[irq_number](IRQ_DETECT_ACK) = 0;
    // do not work!
    gpio(irq_number, IRQ_DETECT_ACK) &= -1;
}

void GPIO::enable_interrupt(Edge e, Handler * h, bool power_up, Edge power_up_edge)
{
    IC::disable(irq());
//    IC::unpend(irq());
    disable_interrupt();
    clear_interrupt();
    IC::int_vector(IC::irq2int(irq()), GPIO::gpio_int_handler);
    gpio(_port, IS) &= ~_pin_bit; // Set interrupt to edge-triggered
    if(e == BOTH_EDGES)
        gpio(_port, IBE) |= _pin_bit; // Interrupt on both edges
    else
    {
        gpio(_port, IBE) &= ~_pin_bit; // Interrupt on single edge, defined by IEV
        if(e == RISING_EDGE)
            gpio(_port, IEV) |= _pin_bit; // Interrupt on rising edge
        else if(e == FALLING_EDGE)
            gpio(_port, IEV) &= ~_pin_bit; // Interrupt on falling edge
    }

    _user_handler = h;
    requester_pin[_port][_pin_number] = this;

    clear_interrupt();
    gpio(_port, IM) |= _pin_bit; // Enable interrupts for this pin 

//    if(Cortex_M_Model::supports_gpio_power_up and power_up) {
//        switch(power_up_edge) {
//            case FALLING_EDGE:
//                gpio(_port, P_EDGE_CTRL) |= (_pin_bit << (8*irq()));
//                break;
//            default:
//                db<GPIO>(WRN) << "GPIO::enable_interrupt: Power up interrupt must choose either rising or falling edge! defaulting to rising." << endl;
//            case RISING_EDGE:
//                gpio(_port, P_EDGE_CTRL) &= ~(_pin_bit << (8*irq()));
//                break;
//        }
//        gpio(_port, PI_IEN) |= (_pin_bit << (8*irq()));
//    }

    IC::enable(irq());
}

__END_SYS
