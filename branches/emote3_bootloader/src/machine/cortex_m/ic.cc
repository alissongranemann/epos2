// EPOS Cortex_M IC Mediator Implementation

#include <machine/cortex_m/ic.h>
#include <machine.h>
#include <machine/cortex_m/bootloader.h>

extern "C" { void _exit(int s); }
extern "C" { void _int_dispatch() __attribute__ ((alias("_ZN4EPOS1S11Cortex_M_IC8dispatchEv"))); }

__BEGIN_SYS

// Class attributes
Cortex_M_IC::Interrupt_Handler Cortex_M_IC::_int_vector[Cortex_M_IC::INTS];

// Class methods
// We need a naked wrapper to dispatch because the compiler doesn't see the 
// register pushes when reserving stack space in the prologue, and this 
// was causing registers to be corrupted
void Cortex_M_IC::dispatch()
{
    // The processor pushes r0-r3, r12, lr, pc, psr and eventually an alignment before getting here, so we just save r4-r11
    // lr is pushed again because the processor updates it with a code which when loaded to pc signals exception return
    ASM("push {lr}\n"
        "push {r4-r11}\n"
        "bl _ZN4EPOS1S11Cortex_M_IC9_dispatchEv\n"
        "pop {r4-r11}\n"
        "pop {pc}\n");
}

void Cortex_M_IC::_dispatch()
{
    if(Cortex_M_Bootloader::bootloader_finished())
    {
        Cortex_M_Bootloader::jump_to_epos_int_handler();
        return;
    }

    // The function is compiled without omitting frame stacking because the compiler saves what it uses

    register Interrupt_Id id = CPU::int_id();

    if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::dispatch(i=" << id << ")" << endl;

    _int_vector[id](id);
}

void Cortex_M_IC::int_not(const Interrupt_Id & i)
{
    db<IC>(ERR) << "IC::int_not(i=" << i << ")" << endl;
}

__END_SYS
