#include <gpio.h>
#include <ic.h>
#include <machine/cortex_m/timer.h>

__BEGIN_SYS

GPIO * GPIO::requester_pin[4][8];

void GPIO::disable_interrupt()
{
    reg(IM) &= ~_pin_bit;    
}

void GPIO::gpio_int_handler(const IC::Interrupt_Id & int_number)
{
    auto irq_number = IC::int2irq(int_number);

    typedef volatile Reg32& (*Reg_Function)(unsigned int);
    Reg_Function regs[] = {gpioa, gpiob, gpioc, gpiod};

    for (auto i = 0u; i < 8; ++i) {
        const bool regular_interrupt = regs[irq_number](MIS) & (1 << i);
        const bool power_up_interrupt = regs[irq_number](IRQ_DETECT_ACK) & ((1 << i) << (8*irq_number));
        if (regular_interrupt or power_up_interrupt) {
            auto pin = requester_pin[irq_number][i];
            if(pin) {
                if(pin->_user_handler) {
                    (*(pin->_user_handler))(requester_pin[irq_number][i]);
                }
            }
        }
    }
    // Clear regular interrupts even if no handler is available
    regs[irq_number](ICR) = -1;

    // Clear power-up interrupts even if no handler is available
    // There is something weird going on here.
    // The manual says: "There is a self-clearing function to this register that generates a
    // reset pulse to clear any interrupt which has its corresponding bit set to 1."
    // But this is not happening! 
    // Also, clearing only the bit that is set or replacing the statement below with
    // regs[irq_number](IRQ_DETECT_ACK) = 0;
    // do not work!
    regs[irq_number](IRQ_DETECT_ACK) &= -1;
}

void GPIO::enable_interrupt(Edge e, GPIO_Handler h, bool power_up, Edge power_up_edge)
{
    IC::disable(_irq);
    IC::unpend(_irq);
    disable_interrupt();
    clear_interrupt();
    IC::int_vector(IC::irq2int(_irq), GPIO::gpio_int_handler);
    reg(IS) &= ~_pin_bit; // Set interrupt to edge-triggered
    if(e == BOTH_EDGES)
        reg(IBE) |= _pin_bit; // Interrupt on both edges
    else
    {
        reg(IBE) &= ~_pin_bit; // Interrupt on single edge, defined by IEV
        if(e == RISING_EDGE)
            reg(IEV) |= _pin_bit; // Interrupt on rising edge
        else if(e == FALLING_EDGE)
            reg(IEV) &= ~_pin_bit; // Interrupt on falling edge
    }

    _user_handler = h;
    requester_pin[_irq][_pin_number] = this;

    clear_interrupt();
    reg(IM) |= _pin_bit; // Enable interrupts for this pin 

    if(power_up) {
        switch(power_up_edge) {
            case FALLING_EDGE:
                reg(P_EDGE_CTRL) |= (_pin_bit << (8*_irq));
                break;
            default:
                db<GPIO>(WRN) << "GPIO::enable_interrupt: Power up interrupt must choose either rising or falling edge! defaulting to rising." << endl;
            case RISING_EDGE:
                reg(P_EDGE_CTRL) &= ~(_pin_bit << (8*_irq));
                break;
        }
        reg(PI_IEN) |= (_pin_bit << (8*_irq));
    }

    IC::enable(_irq);
}

__END_SYS
