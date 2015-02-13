// EPOS eMote3 (Cortex-M3) MCU Initialization

#include <machine/cortex_m/emote3.h>

__BEGIN_SYS

void eMote3::init()
{
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
    }
    // Select IO and system oscilators
    // Delay qualification of crystal oscillator (XOSC) until it is stable
    scr(CLOCK_CTRL) = AMP_DET | (SYS_DIV * clock_val) | OSC32K | (IO_DIV * clock_val); 

    // Wait until oscillator stabilizes
    while((scr(CLOCK_STA) & (STA_OSC)));

    // Enable alternate interrupt mapping
    scr(I_MAP) |= I_MAP_ALTMAP;
}

__END_SYS
