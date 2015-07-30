#include <system/config.h>
#include __MODEL_H
#ifdef __emote3_h

#include <ic.h>

__BEGIN_SYS

GPIO * GPIO::requester_pin[4][8];

void GPIO::disable_interrupt()
{
    reg(IM) &= ~_pin_bit;
}

void GPIO::clear_interrupt()
{
    reg(ICR) |= _pin_bit;
}

void GPIO::gpio_int_handler(const IC::Interrupt_Id & int_number)
{
    return;
    auto irq_number = IC::int2irq(int_number);
    typedef volatile Reg32& (*Reg_Function)(unsigned int);
    Reg_Function regs[] = {gpioa, gpiob, gpioc, gpiod};
    for (auto i = 0u; i < 8; ++i) {
        if (regs[irq_number](MIS) & (1 << i)) {
            (*(requester_pin[irq_number][i]->_user_handler))(requester_pin[irq_number][i]);
            requester_pin[irq_number][i]->clear_interrupt();
        }
    }
}

void GPIO::enable_interrupt(Edge e, GPIO_Handler h)
{
    IC::disable(_irq);
    IC::unpend(_irq);
    disable_interrupt();
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
    IC::enable(_irq);
}

__END_SYS
#endif
