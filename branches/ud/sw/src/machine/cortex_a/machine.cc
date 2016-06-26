// EPOS Cortex_A Mediator Implementation

#include <machine/cortex_a/machine.h>
#include <display.h>

__BEGIN_SYS

volatile unsigned int Cortex_A::_n_cpus;

void Cortex_A::panic()
{
    CPU::int_disable();
    if(Traits<Display>::enabled)
        Display::puts("PANIC!\n");
    if(Traits<System>::reboot)
        reboot();
    else
        CPU::halt();
}

void Cortex_A::reboot()
{
    db<Machine>(WRN) << "Machine::reboot()" << endl;
    Cortex_A_Model::reboot();
}

__END_SYS

