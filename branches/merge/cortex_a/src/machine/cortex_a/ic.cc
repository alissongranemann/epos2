// EPOS Cortex-A IC Mediator Implementation

#include <machine/cortex_a/ic.h>

//extern "C" { void _exit(int s); }
// The _dispatch alias shouldn't be necessary but the assembler throws an error
// when passing Cortex_A_IC::dispatch() as an ASM input operand
extern "C" { void _dispatch() __attribute__ ((alias("_ZN4EPOS1S11Cortex_A_IC8dispatchEv"))); }
extern "C" { void _int_entry() __attribute__ ((alias("_ZN4EPOS1S11Cortex_A_IC5entryEv"))); }

__BEGIN_SYS

// Class attributes
Cortex_A_IC::Interrupt_Handler Cortex_A_IC::_int_vector[Cortex_A_IC::INTS];

// Class methods
// TODO: Document why this mess is necessary
void Cortex_A_IC::entry()
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
        "ldmfd sp!, {r0-r3, r12, lr, pc}^           \n");
}

void Cortex_A_IC::int_not(const Interrupt_Id & i)
{
    db<IC>(WRN) << "IC::int_not(i=" << i << ")" << endl;
}

__END_SYS
