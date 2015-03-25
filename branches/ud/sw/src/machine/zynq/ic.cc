// EPOS Zynq IC Mediator Implementation

#include <machine/zynq/ic.h>

extern "C" { void _exit(int s); }
extern "C" { void _int_dispatch() __attribute__ ((alias("_ZN4EPOS1S7Zynq_IC8dispatchEv"))); }

__BEGIN_SYS

// Class attributes
Zynq_IC::Interrupt_Handler Zynq_IC::_int_vector[Zynq_IC::INTS];

// Class methods
void Zynq_IC::dispatch()
{
    unsigned int icciar = cpu_itf(ICCIAR);
    register Interrupt_Id id = icciar & INT_ID_MASK;

    if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::dispatch(i=" << id << ")" << endl;

    _int_vector[id](id);

    // For every read of a valid interrupt id from the ICCIAR, the ISR must
    // perform a matching write to the ICCEOIR
    cpu_itf(ICCEOIR) = icciar;
}

void Zynq_IC::int_not(const Interrupt_Id & i)
{
    db<IC>(WRN) << "IC::int_not(i=" << i << ")" << endl;
}

__END_SYS
