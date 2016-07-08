// EPOS EPOSMote-III Initialization

#include <machine/cortex_m/emote3.h>

__BEGIN_SYS

void eMote3::init()
{
    db<Init, Cortex_M>(TRC) << "eMote3::init()" << endl;

    CPU::Reg32 clock = scr(CLOCK_CTRL);
    // Delay qualification of crystal oscillator (XOSC) until it is stable
    clock |= AMP_DET;

    // Select 32k RC oscillator and 32MHz crystal system oscillator
    unsigned int val = clock & ~(OSC32K | OSC | (SYS_DIV * 7));
    val |= OSC32K;
    clock = val;

    // Wait until oscillator stabilizes
    while((scr(CLOCK_STA) & (STA_OSC)));

    // Set IO clock to 32MHz
    clock &= ~(IO_DIV * 7);
}

__END_SYS
