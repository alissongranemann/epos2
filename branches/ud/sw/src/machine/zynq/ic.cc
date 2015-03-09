// EPOS Zynq IC Mediator Implementation

#include <machine/zynq/ic.h>

__BEGIN_SYS

// Class attributes
Zynq_IC::Interrupt_Handler Zynq_IC::_int_vector[Zynq_IC::INTS];

// Class methods
void Zynq_IC::dispatch()
{
    // TODO: Save context?

    // Interrupt Acknowledge Register (ICCIAR)
    unsigned int icciar_value = CPU::in32(PROC_INTERFACE | ICCIAR);
    IC::Interrupt_Id id = icciar_value & INTERRUPT_MASK; // 0x3FF bits 0 to 9

    if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::dispatch(i=" << id << ")" << endl;

    _int_vector[id](id);

    // For every read of a valid Interrupt ID from the ICCIAR, the interrupt
    // service routine on the connected processor must perform a matching write
    // to the ICCEOIR, see End of Interrupt Register (ICCEOIR)
    CPU::int_disable();
    CPU::out32(PROC_INTERFACE | ICCEOI, icciar_value);
}

void Zynq_IC::int_not(const Interrupt_Id & i)
{
    db<IC>(WRN) << "IC::int_not(i=" << i << ")" << endl;
}

__END_SYS
