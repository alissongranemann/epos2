// EPOS EPOSSOC Mediator Implementation

#include <machine.h>
////extern "C" { void _exit(int s); }

__BEGIN_SYS

// Class attributes
void EPOSSOC::panic() {
    db<EPOSSOC>(ERR) << "PANIC!\n";
	CPU::int_disable();
	*((volatile unsigned int*)0xFFFFFFFC) = 0;
	CPU::halt();
}

__END_SYS

