// EPOS Cortex Mediator Implementation

#include <machine/cortex/machine.h>
#include <display.h>

__BEGIN_SYS

void Cortex::panic()
{
    CPU::int_disable();
    if(Traits<Display>::enabled)
        Display::puts("PANIC!\n");
    if(Traits<System>::reboot)
        reboot();
    else
        CPU::halt();
}

void Cortex::reboot()
{
    db<Machine>(WRN) << "Machine::reboot()" << endl;
    Cortex_Model::reboot();
}

__END_SYS
