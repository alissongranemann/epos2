// EPOS Cortex_M Mediator Implementation

#include <machine/cortex_m/machine.h>

__BEGIN_SYS

void Cortex_M::init()
{
    db<Init, Cortex_M>(TRC) << "Cortex_M::init()" << endl;

    CPU::Reg32 rcc = scr(RCC);

    // bypass PLL and system clock divider while initializing
    rcc |= RCC_BYPASS;
    rcc &= ~RCC_USESYSDIV;
    scr(RCC) = rcc;

    // select the crystal value and oscillator source
    rcc &= ~RCC_XTAL_8192;
    rcc |= RCC_XTAL_6000;
    rcc |= RCC_MOSC;

    // activate PLL by clearing PWRDN and OEN
    rcc &= ~RCC_PWRDN;
    rcc &= ~RCC_OEN;

    // set the desired system divider and the USESYSDIV bit
    rcc |= RCC_SYSDIV_4;
    rcc |= RCC_USESYSDIV;
    scr(RCC) = rcc;

    // wait for the PLL to lock by polling PLLLRIS
    PLL_Init_loop:
    CPU::Reg32 ris = scr(RIS);
    ris &= RIS_PLLLRIS;
    if(ris)
        goto PLL_Init_loop;

    // enable use of PLL by clearing BYPASS
    rcc &= ~RCC_BYPASS;
    scr(RCC) = rcc;

    if(Traits<Cortex_M_IC>::enabled)
        Cortex_M_IC::init();
//    if(Traits<Cortex_M_Bus>::enabled)
//        Cortex_M_Bus::init();
    if(Traits<Cortex_M_Timer>::enabled)
        Cortex_M_Timer::init();
//    if(Traits<Cortex_M_Scratchpad>::enabled)
//        Cortex_M_Scratchpad::init();
}

__END_SYS
