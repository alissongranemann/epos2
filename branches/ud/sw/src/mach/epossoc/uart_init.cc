// EPOS EPOSSOC UART Mediator Initialization

#include <machine.h>

__BEGIN_SYS

void EPOSSOC_UART::init()
{
    db<EPOSSOC_UART>(TRC) << "EPOSSOC_UART::init()\n";

	//Set configured BAUDRATE;
	_regs->clkdiv = CLOCK/BAUDRATE;

}

__END_SYS
