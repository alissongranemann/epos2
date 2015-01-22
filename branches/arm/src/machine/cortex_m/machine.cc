// EPOS Cortex_M Mediator Implementation

#include <machine/cortex_m/machine.h>
#include <display.h>

__BEGIN_SYS

void Cortex_M::panic()
{
    CPU::int_disable();
    Display::puts("PANIC!\n");
    if(Traits<System>::reboot)
        reboot();
    else
        CPU::halt();
}
__END_SYS
