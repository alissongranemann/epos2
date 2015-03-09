// EPOS Zynq Mediator Initialization

#include <machine/zynq/machine.h>

__BEGIN_SYS

void Zynq::init()
{
    db<Init, Zynq>(TRC) << "Zynq::init()" << endl;

    // Unlock SLCR and reset FPGA
    CPU::out32(0xF8000008, 0xDF0D);
    CPU::out32(0xF8000240, 0xF);
    CPU::out32(0xF8000240, 0x0);

    if(Traits<Zynq_IC>::enabled)
        Zynq_IC::init();
    if(Traits<Zynq_Timer>::enabled)
        Zynq_Timer::init();
}

__END_SYS
