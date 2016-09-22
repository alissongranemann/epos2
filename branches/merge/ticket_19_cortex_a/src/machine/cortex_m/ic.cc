// EPOS Cortex_M IC Mediator Implementation

#include <machine/cortex_m/ic.h>
#include <machine.h>

//extern "C" { void _exit(int s); }
extern "C" { void _int_entry() __attribute__ ((alias("_ZN4EPOS1S2IC5entryEv"))); }

__BEGIN_SYS

// Class attributes
IC::Interrupt_Handler IC::_int_vector[IC::INTS];

// Class methods
void IC::entry() // __attribute__((naked));
{
    // The processor pushes r0-r3, r12, lr, pc, psr and eventually an alignment before getting here, so we just save r4-r11
    // lr is pushed again because the processor updates it with a code which when loaded to pc signals exception return
    ASM("	push	{lr}		\n"
        "	push	{r4-r11}	\n"
        "	bl		%0			\n"
        "	pop		{r4-r11}	\n"
        "	pop 	{pc}		\n" : : "i"(dispatch));
}

void IC::int_not(const Interrupt_Id & i)
{
    db<IC>(WRN) << "IC::int_not(i=" << i << ")" << endl;
}

void IC::hard_fault(const Interrupt_Id & i)
{
    db<IC>(ERR) << "IC::hard_fault(i=" << i << ")" << endl;
    Machine::panic();
}

__END_SYS

