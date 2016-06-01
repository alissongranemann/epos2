// EPOS Cortex-A Mediator Initialization

#include <machine/cortex_a/machine.h>

__BEGIN_SYS

void Cortex_A::init()
{
    db<Init, Cortex_A>(TRC) << "Cortex_A::init()" << endl;

    // Unlock SLCR and reset FPGA
    CPU::out32(0xF8000008, 0xDF0D);
    CPU::out32(0xF8000240, 0xF);
    CPU::out32(0xF8000240, 0x0);

    if(Traits<Cortex_A_IC>::enabled)
        Cortex_A_IC::init();
    if(Traits<Cortex_A_Timer>::enabled)
        Cortex_A_Timer::init();
}

__END_SYS
