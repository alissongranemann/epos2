// EPOS Cortex IC Mediator Implementation

#include <machine/cortex/ic.h>
#include <machine.h>

//extern "C" { void _exit(int s); }
extern "C" { void _int_entry() __attribute__ ((alias("_ZN4EPOS1S9Cortex_IC5entryEv"))); }
extern "C" { void _dispatch() __attribute__ ((alias("_ZN4EPOS1S9Cortex_IC8dispatchEj"))); }

__BEGIN_SYS

// Class attributes
Cortex_IC::Interrupt_Handler Cortex_IC::_int_vector[Cortex_IC::INTS];

// Class methods
#ifdef __mmod_zynq__

void Cortex_IC::entry()
{
    ASM(".equ MODE_IRQ, 0x12                        \n"
        ".equ MODE_SVC, 0x13                        \n"
        ".equ IRQ_BIT,  0x80                        \n"
        ".equ FIQ_BIT,  0x40                        \n"
        // Go to SVC
        "msr cpsr_c, #MODE_SVC | IRQ_BIT | FIQ_BIT  \n"
        // Save current context (lr, sp and spsr are banked registers)
        "stmfd sp!, {r0-r3, r12, lr, pc}            \n"
        // Go to IRQ
        "msr cpsr_c, #MODE_IRQ | IRQ_BIT | FIQ_BIT  \n"
        // Return from IRQ address
        "sub r0, lr, #4                             \n"
        // Pass irq_spsr to SVC r1
        "mrs r1, spsr                               \n"
        // Go back to SVC
        "msr cpsr_c, #MODE_SVC | IRQ_BIT | FIQ_BIT  \n"
        // sp+24 is the position of the saved pc
        "add r2, sp, #24                            \n"
        // Save address to return from interrupt into the pc position to retore
        // context later on
        "str r0, [r2]                               \n"
        // Save IRQ-spsr
        "stmfd sp!, {r1}                            \n"
        //"bl %0                                      \n"
        "bl _dispatch                               \n"
        "ldmfd sp!, {r0}                            \n"
        // Restore IRQ's spsr value to SVC's spsr
        "msr spsr_cfxs, r0                          \n"
        // Restore context, the ^ in the end of the above instruction makes the
        // irq_spsr to be restored into svc_cpsr
        "ldmfd sp!, {r0-r3, r12, lr, pc}^           \n" : : "i"(dispatch));
}

#else

void Cortex_IC::entry()
{
    // The processor pushes r0-r3, r12, lr, pc, psr and eventually an alignment before getting here, so we just save r4-r11
    // lr is pushed again because the processor updates it with a code which when loaded to pc signals exception return
    ASM("       push    {lr}            \n"
        "       push    {r4-r11}        \n"
        "       bl      %0              \n"
        "       pop     {r4-r11}        \n"
        "       pop     {pc}            \n" : : "i"(dispatch));
}

#endif

void Cortex_IC::int_not(const Interrupt_Id & i)
{
    db<IC>(WRN) << "IC::int_not(i=" << i << ")" << endl;
}

void Cortex_IC::hard_fault(const Interrupt_Id & i)
{
    db<IC>(ERR) << "IC::hard_fault(i=" << i << ")" << endl;
    Machine::panic();
}

__END_SYS

