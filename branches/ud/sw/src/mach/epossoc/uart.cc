// EPOS EPOSSOC UART Mediator Implementation

#include <uart.h>

__BEGIN_SYS

EPOSSOC_UART::MM_Reg * EPOSSOC_UART::_regs = reinterpret_cast<EPOSSOC_UART::MM_Reg *>(EPOSSOC_UART::BASE_ADDRESS);

void EPOSSOC_UART::int_handler(unsigned int interrupt){
    db<EPOSSOC_UART>(TRC) << "EPOSSOC_UART::int_handler(intr=" << interrupt << ")\n";
	// To be implemented.
	
}

__END_SYS
