// EPOS PC Mediator Implementation

#include <machine/pc/machine.h>

__BEGIN_SYS

// Class attributes
volatile unsigned int PC::_n_cpus;

// Class methods
void PC::panic()
{
    CPU::int_disable(); 
    Display::position(24, 73);
    Display::puts("PANIC!");
    if(Traits<System>::reboot)
        Machine::reboot();
    else
        CPU::halt();
}

__END_SYS
