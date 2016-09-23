// EPOS Cortex IC Mediator Implementation

#include <machine/cortex/ic.h>
#include <machine.h>

//extern "C" { void _exit(int s); }
extern "C" { void _int_entry() __attribute__ ((alias("_ZN4EPOS1S2IC5entryEv"))); }
extern "C" { void _dispatch() __attribute__ ((alias("_ZN4EPOS1S2IC8dispatchEj"))); }

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

#else

void IC::entry()
{

    ASM("   mrs     r0, xpsr           \n"
        "   and     r0, #0x3f          \n"); // Store int_id in r0 (which will be passed as argument to dispatch())

    if(Traits<USB>::enabled) {
        // This is a workaround for the USB interrupt (60). It is level-enabled, so we need to process it in handler mode, otherwise the handler will never exit
        ASM("   cmp     r0, #60            \n" // Check if this is the USB IRQ
            "   bne     NOT_USB            \n"
            "   b       _dispatch          \n" // Execute USB interrupt in handler mode
            "   bx      lr                 \n" // Return from handler mode directly to pre-interrupt code
            "NOT_USB:                      \n");
    }

    ASM("   mov     r3, #1             \n"
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
        "   isb                        \n"
        "   pop {pc}                   \n");// Pops EXC_RETURN, so that stack is in state (1)
                                            // Load EXC_RETURN code to pc
                                            // Processor unrolls stack (1)
                                            // And we're back to pre-interrupt code
}

#endif

void IC::dispatch(unsigned int i) {
    Interrupt_Id id = int_id();

    if((id != INT_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::dispatch(i=" << id << ")" << endl;

    _int_vector[id](id);
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

