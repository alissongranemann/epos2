// EPOS EPOSSOC Mediator Initialization

#include <machine.h>
#include <system.h>
#include <uart.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

void EPOSSOC::init()
{
    db<EPOSSOC>(TRC) << "EPOSSOC::init()\n";

    if(Traits<EPOSSOC_IC>::enabled)
	EPOSSOC_IC::init();
    if(Traits<EPOSSOC_Timer>::enabled)
	EPOSSOC_Timer::init();
    if(Traits<EPOSSOC_UART>::enabled)
	EPOSSOC_UART::init();
    if(Traits<EPOSSOC_NIC>::enabled)
	EPOSSOC_NIC::init();
}

__END_SYS
