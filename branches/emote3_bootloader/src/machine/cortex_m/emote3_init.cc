// EPOS eMote3 (Cortex-M3) MCU Initialization

#include <system/config.h>
#include __MODEL_H
#ifdef __emote3_h

#include <ic.h>

__BEGIN_SYS

// eMote3 TSC static values
CC2538_TSC::Time_Stamp CC2538_TSC::_offset = 0;
bool CC2538_TSC::_positive_offset;
bool eMote3::_init_clock_done = false;
GPIO * GPIO::requester_pin[4];

void GPIO::disable_interrupt()
{ 
    reg(IM) &= ~_pin_bit; 
}

void GPIO::enable_interrupt(Edge e, GPIO_Handler h)
{
    disable_interrupt();
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
    requester_pin[_irq] = this;

    reg(IM) |= _pin_bit; // Enable interrupts for this pin
}

void eMote3::init()
{
    init_clock();

    // Enable alternate interrupt mapping
    scr(I_MAP) |= I_MAP_ALTMAP;

    // IC::int_vector(0, GPIO::gpio_int_handler);
    // IC::int_vector(1, GPIO::gpio_int_handler);
    // IC::int_vector(2, GPIO::gpio_int_handler);
    // IC::int_vector(3, GPIO::gpio_int_handler);
    // IC::enable(0);
    // IC::enable(1);
    // IC::enable(2);
    // IC::enable(3);
    
}

void eMote3::init_clock()
{
    // Since the clock is configured in traits and never changes,
    // this needs to be done only once, but this method will be 
    // called at least twice during EPOS' initialization 
    // (in eMote3::config_UART() and Cortex_M::init())
    if(_init_clock_done)
        return;

    // Clock setup
    Reg32 clock_val;    
    switch(Traits<CPU>::CLOCK)
    {
        case 32000000: clock_val = 0; break;
        case 16000000: clock_val = 1; break;
        case  8000000: clock_val = 2; break;
        case  4000000: clock_val = 3; break;
        case  2000000: clock_val = 4; break;
        case  1000000: clock_val = 5; break;
        case   500000: clock_val = 6; break;
        case   250000: clock_val = 7; break;
        default: while(1) assert(false);
    }
    // Enable AMP detect to make sure XOSC starts correctly
    scr(CLOCK_CTRL) |= AMP_DET;

    Reg32 clock_ctrl = scr(CLOCK_CTRL) & ~(SYS_DIV * 7);
    clock_ctrl &= ~OSC; // Select 32Mhz oscillator

    clock_ctrl &= ~OSC32K; // Select 32Khz crystal oscillator
    clock_ctrl |= clock_val * SYS_DIV; // Set system clock rate

    scr(CLOCK_CTRL) = clock_ctrl;

    // Wait until oscillator stabilizes
    while((scr(CLOCK_STA) & (STA_OSC)));

    clock_ctrl = scr(CLOCK_CTRL) & ~(IO_DIV * 7);
    scr(CLOCK_CTRL) |= clock_val * IO_DIV; // Set IO clock rate

    _init_clock_done = true;
}

__END_SYS
#endif
