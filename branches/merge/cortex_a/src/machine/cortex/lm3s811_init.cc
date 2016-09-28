// EPOS LM3S811 (ARM Cortex) MCU Initialization

#include <system/config.h>
#include <machine.h>

#ifdef __mmod_lm3s811__

__BEGIN_SYS

bool Machine_Model::_init_clock_done = false;

void Machine_Model::init()
{
    db<Init, Machine>(TRC) << "Machine_Model::init()" << endl;

    init_clock();

    db<Init, Machine>(TRC) << "Machine_Model::init:CCR = " << scs(CCR) << endl;
    scs(CCR) |= BASETHR; // BUG: on LM3S811 this register is not updated, but it doesn't seem to cause any errors
    db<Init, Machine>(TRC) << "Machine_Model::init:CCR = " << scs(CCR) << endl;
}

void Machine_Model::init_clock()
{
    // Since the clock is configured in traits and never changes,
    // this needs to be done only once, but this method will be
    // called at least twice during EPOS' initialization
    // (in LM3S811::enable_uart() and Machine::init())
    if(_init_clock_done)
        return;

    // Initialize the clock
    CPU::Reg32 rcc = scr(RCC);

    // bypass PLL and system clock divider while initializing
    rcc |= RCC_BYPASS;
    rcc &= ~RCC_USESYSDIV;
    scr(RCC) = rcc;

    // select the crystal value and oscillator source
    rcc &= ~RCC_XTAL_8192;
    rcc |= RCC_XTAL_6000;
    rcc &= ~RCC_IOSC4;
    rcc |= RCC_MOSC;

    // activate PLL by clearing PWRDN and OEN
    rcc &= ~RCC_PWRDN;
    rcc &= ~RCC_OEN;

    // set the desired system divider and the USESYSDIV bit
    rcc &= ~RCC_SYSDIV_16;
    // Clock setup
    Reg32 sys_div;
    switch(Traits<CPU>::CLOCK) {
        default:
        case 50000000: sys_div = RCC_SYSDIV_4; break;
        case 40000000: sys_div = RCC_SYSDIV_5; break;
        case 25000000: sys_div = RCC_SYSDIV_8; break;
        case 20000000: sys_div = RCC_SYSDIV_10; break;
        case 12500000: sys_div = RCC_SYSDIV_16; break;
    }
    rcc |= sys_div;
    rcc |= RCC_USESYSDIV;
    scr(RCC) = rcc;

    // wait for the PLL to lock by polling PLLLRIS
    while(!(scr(RIS) & RIS_PLLLRIS));

    // enable use of PLL by clearing BYPASS
    rcc &= ~RCC_BYPASS;
    scr(RCC) = rcc;

    _init_clock_done = true;
}

__END_SYS

#endif
