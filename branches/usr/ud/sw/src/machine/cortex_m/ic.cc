// EPOS Cortex_M IC Mediator Implementation

#include <machine/cortex_m/ic.h>
#include <machine.h>

extern "C" { void _exit(int s); }
extern "C" { void _int_dispatch() __attribute__ ((alias("_ZN4EPOS1S11Cortex_M_IC8dispatchEv"))); }

__BEGIN_SYS

// Class attributes
Cortex_M_IC::Interrupt_Handler Cortex_M_IC::_int_vector[Cortex_M_IC::INTS];


// Class methods
void Cortex_M_IC::dispatch()
{
    // The processor pushes r0-r3, r12, lr, pc, psr and eventually an alignment before getting here, so we just save r4-r11
    // The function is compiled without omitting frame stacking because the compiler saves what it uses from r4-r11
    ASM("push {r4-r11}");

    register Interrupt_Id id = CPU::int_id();

    if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::dispatch(i=" << id << ")" << endl;

    _int_vector[id](id);

    ASM("pop {r4-r11}");
}

void Cortex_M_IC::int_not(const Interrupt_Id & i)
{
    db<IC>(WRN) << "IC::int_not(i=" << i << ")" << endl;
}

__END_SYS

