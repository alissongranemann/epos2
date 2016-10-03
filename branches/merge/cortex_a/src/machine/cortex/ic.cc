// EPOS Cortex IC Mediator Implementation

#include <machine/cortex/ic.h>
#include <machine.h>

extern "C" { void _int_entry() __attribute__ ((alias("_ZN4EPOS1S2IC5entryEv"))); }
extern "C" { void _dispatch(unsigned int) __attribute__ ((alias("_ZN4EPOS1S2IC8dispatchEj"))); }
extern "C" { void _eoi(unsigned int) __attribute__ ((alias("_ZN4EPOS1S2IC3eoiEj"))); }
extern "C" { void _undefined_instruction() __attribute__ ((alias("_ZN4EPOS1S2IC21undefined_instructionEv"))); }
extern "C" { void _software_interrupt() __attribute__ ((alias("_ZN4EPOS1S2IC18software_interruptEv"))); }
extern "C" { void _prefetch_abort() __attribute__ ((alias("_ZN4EPOS1S2IC14prefetch_abortEv"))); }
extern "C" { void _data_abort() __attribute__ ((alias("_ZN4EPOS1S2IC10data_abortEv"))); }
extern "C" { void _reserved() __attribute__ ((alias("_ZN4EPOS1S2IC8reservedEv"))); }
extern "C" { void _fiq() __attribute__ ((alias("_ZN4EPOS1S2IC3fiqEv"))); }

__BEGIN_SYS

// Class attributes
IC::Interrupt_Handler IC::_int_vector[IC::INTS];

// Class methods
#ifdef __mmod_zynq__

void IC::entry()
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

void IC::dispatch(unsigned int i)
{
    Interrupt_Id id = int_id();

    if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::dispatch(i=" << id << ")" << endl;

    assert(id < INTS);
    if(_eoi_vector[id])
        _eoi_vector[id](id);

    CPU::int_enable();

    _int_vector[id](id);
}

#else
/*
We need to get around Cortex M3's interrupt handling to be able to make it re-entrant
The problem is that interrupts are handled in Handler mode, and in this mode the processor
is only preempted by interrupts of higher (not equal!) priority.
Moreover, the processor automatically pushes information into the stack when an interrupt happens,
and it only pops this information and gets out of Handler mode when a specific value (called EXC_RETURN)
is loaded to PC, representing the final return from the interrupt handler.
When an interrupt happens, the processor pushes this information into the current stack:

   (1) Stack pushed by processor
         +-----------+
SP + 32  |<alignment>| (one word of padding if necessary, to make the stack 8-byte-aligned)
         +-----------+
SP + 28  |xPSR       | (with bit 9 set if there is alignment)
         +-----------+
SP + 24  |PC         | (return address)
         +-----------+
SP + 20  |LR         | (link register before being overwritten with EXC_RETURN)
         +-----------+
SP + 16  |R12        | (general purpose register 12)
         +-----------+
SP + 12  |R3         | (general purpose register 3)
         +-----------+
SP +  8  |R2         | (general purpose register 2)
         +-----------+
SP +  4  |R1         | (general purpose register 1)
         +-----------+
SP       |R0         | (general purpose register 0)
         +-----------+

Also, it enters Handler mode and the value of LR is overwritten with EXC_RETURN
(in our case, it is always 0xFFFFFFF9).
To execute dispatch() in Thread mode, which is preemptable, we extend this stack with the following:

   (2) Stack built to make the processor execute dispatch() outside of Handler mode
         +-----------+
SP + 32  |EXC_RETURN | (value used later on)
         +-----------+
SP + 28  |1 << 24    | (xPSR with Thumb bit set (the only mandatory bit for Cortex-M3))
         +-----------+
SP + 24  |dispatch   | (address of the actual dispatch method)
         +-----------+
SP + 20  |exit       | (address of the interrupt epilogue)
         +-----------+
SP + 16  |Don't Care | (general purpose register 12)
         +-----------+
SP + 12  |Don't Care | (general purpose register 3)
         +-----------+
SP +  8  |Don't Care | (general purpose register 2)
         +-----------+
SP +  4  |Don't Care | (general purpose register 1)
         +-----------+
SP       |int_id     | (to be passed as argument to dispatch())
         +-----------+

And then load EXC_RETURN into pc. This will cause stack (2) to the popped up until the EXC_RETURN value pushed.
The stack will return to state (1) with the addition of EXC_RETURN, the processor will be in Thread mode, and
the followingregisters of interest will be updated:
    r0 = int_id
    pc = dispatch
    lr = exit

Then dispatch(int_id) will be executed and return to _int_exit, which simply issues a supervisor call (SVC).
The processor then enters handler mode and pushes a new stack like (1) to execute the SVC. The svc handler
simply ignores this stack, sets the stack back to (1) and returns from the interrupt, making the processor
restore the context it saved in (1).

We use SVC to return because the processor does things when returning from an interrupt that are hard to be
replicated in software. For instance, it might consistently return to the middle (not the beginning) of
an stm (Store Multiple) instruction.

Known issues:
- If the handler executed disables interrupts, the svc instruction in _int_exit will cause a hard fault.
This can be detected and revert if necessary. One would need to make the hard fault handler detect that the
fault was generated in _int_exit, and in this case simply call svc_handler.

More information can be found at:
[1] ARMv7-M Architecture Reference Manual (ARM DDI 0403C_errata_v3 (ID021910), February 2010):
        Section B1.5.6 (Exception entry behavior)
        Section B1.5.7 (Stack alignment on exception entry)
        Section B1.5.8 (Exception return behavior)
[2] https://sites.google.com/site/sippeyfunlabs/embedded-system/how-to-run-re-entrant-task-scheduler-on-arm-cortex-m4
[3] https://community.arm.com/thread/4919
*/
void IC::entry()
{
    ASM("   mrs     r0, xpsr           \n"
        "   and     r0, #0x3f          \n" // Store int_id in r0 (to be passed as argument to eoi() and dispatch())
        "   push    {r0, lr}           \n"
        "   bl      _eoi               \n" // Acknowledge the interrupt
        "   pop     {r0, lr}           \n"
        "   mov     r3, #1             \n"
        "   lsl     r3, #24            \n" // xPSR with Thumb bit only. Other bits are Don't Care
        "   ldr     r1, =_int_exit     \n" // Fake LR (will cause _int_exit to execute after dispatch())
        "   orr     r1, #1             \n"
        "   ldr     r2, =_dispatch     \n" // Fake PC (will cause dispatch() to execute after entry())
        "   sub     r2, #1             \n"
        "   push    {lr}               \n" // Push EXC_RETURN code, which will be popped by svc_handler
        "   push    {r1-r3}            \n" // Fake stack (2): xPSR, PC, LR
        "   push    {r0-r3, r12}       \n" // Push rest of fake stack (2)
        "   bx      lr                 \n" // Return from handler mode. Will proceed to dispatch()
        "_int_exit:                    \n"
        "   svc     #7                 \n" // 7 is an arbitrary number. Will proceed to _svc_handler in handler mode
        ".global _svc_handler          \n"
        "_svc_handler:                 \n" // Set the stack back to state (1) and tell the processor to recover the pre-interrupt context
        "   ldr r0, [sp, #28]          \n" // Read stacked xPSR
        "   ldr r0, [sp, #28]          \n" // Read stacked xPSR
        "   ldr r0, [sp, #28]          \n" // Read stacked xPSR
        "   and r0, #0x200             \n" // Bit 9 indicating alignment existence
        "   lsr r0, #7                 \n" // if bit9==1 then r0=4 else r0=0
        "   add r0, sp                 \n"
        "   add r0, #32                \n" // r0 now points to were EXC_RETURN was pushed
        "   mov sp, r0                 \n" // Set stack pointer to that address
        "   isb                        \n" // Make sure sp is updated before continuing
        "   pop {pc}                   \n");// Pops EXC_RETURN, so that stack is in state (1)
                                            // Load EXC_RETURN code to pc
                                            // Processor unrolls stack (1)
                                            // And we're back to pre-interrupt code
}

void IC::dispatch(unsigned int id)
{
    if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::dispatch(i=" << id << ")" << endl;

    _int_vector[id](id);
}

#endif

void IC::eoi(unsigned int id)
{
    if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::eoi(i=" << id << ")" << endl;

    assert(id < INTS);

    if(_eoi_vector[id])
        _eoi_vector[id](id);
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

void IC::undefined_instruction()
{
    db<IC>(ERR) << "Undefined instruction" << endl;
    Machine::panic();
}

void IC::software_interrupt()
{
    db<IC>(ERR) << "Software interrupt" << endl;
    Machine::panic();
}

void IC::prefetch_abort()
{
    db<IC>(ERR) << "Prefetch abort" << endl;
    Machine::panic();
}

void IC::data_abort()
{
    db<IC>(ERR) << "Data abort" << endl;
    Machine::panic();
}

void IC::reserved()
{
    db<IC>(ERR) << "Reserved" << endl;
    Machine::panic();
}

void IC::fiq()
{
    db<IC>(ERR) << "FIQ handler" << endl;
    Machine::panic();
}

__END_SYS
