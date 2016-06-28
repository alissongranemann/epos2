//#include <ic.h>
//
//__BEGIN_SYS
//
//extern "C" {
//
//void _undefined_instruction() __attribute__ ((naked));
//void _undefined_instruction()
//{
//    kout << "undefined instruction\n";
//    ASM("movs pc, r14");
//}
//
//void _software_interrupt() __attribute__ ((naked));
//void _software_interrupt()
//{
//    kout << "software interrupt\n";
//    ASM("movs pc, r14");
//}
//
//void _prefetch_abort() __attribute__ ((naked));
//void _prefetch_abort()
//{
//    kout << "prefetch abort\n";
//    ASM("subs pc, r14, #4");
//}
//
//void _data_abort() __attribute__ ((naked));
//void _data_abort()
//{
//    kout << "data abort\n";
//    ASM("subs pc, r14, #8");
//}
//
//void _reserved() __attribute__ ((naked));
//void _reserved()
//{
//    kout << "reserved\n";
//    ASM("mov pc, r14");
//}
//
//void _irq_handler() __attribute__ ((naked));
//void _irq_handler() {
//
//    ASM(
//            // A few definitions
//            ".equ ARM_MODE_USR,      0x10 \n"
//            ".equ ARM_MODE_FIQ,      0x11 \n"
//            ".equ ARM_MODE_IRQ,      0x12 \n"
//            ".equ ARM_MODE_SVC,      0x13 \n"
//            ".equ IRQ_BIT,           0x80 \n"
//            ".equ FIQ_BIT,           0x40 \n"
//
//            "msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT \n" // go to SVC
//
//            "stmfd sp!, {r0-r3,r12,lr,pc}\n" // save current context
//
//            "msr cpsr_c, #ARM_MODE_IRQ | IRQ_BIT | FIQ_BIT     \n" // go to IRQ
//
//            "sub r0, lr, #4 \n" // return from irq addr
//            "mrs r1, spsr   \n" // pass irq_spsr to svn r1
//
//            "msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT     \n" // go back to SVC
//            "add r2, sp, #24 \n"
//            "str r0, [r2, #0] \n" // save address to return from interrupt
//            "stmfd sp!, {r1} \n"   // save irq-spsr
//
//
//    );
////    IC::int_handler();
//    ASM(
//            "ldmfd sp!, {r0}              \n"
//            "msr spsr_cfxs, r0           \n" // restore IRQ's spsr value to SVC's spsr
//
//            "ldmfd sp!, {r0-r3,r12,lr,pc}^ \n" // restore context
//        );
//}
//
//void _fiq_handler() __attribute__ ((naked));
//void _fiq_handler()
//{
//    kout << "fiq handler\n";
//    ASM("subs pc, r14, #4");
//}
//
//};
//
#if MACH == cortex_a
#include <ic.h>

__BEGIN_SYS

extern "C" {

void _undefined_instruction() __attribute__ ((naked));
void _undefined_instruction()
{
    kout << "undefined instruction\n";
    ASM("movs pc, r14");
}

void _software_interrupt() __attribute__ ((naked));
void _software_interrupt()
{
    kout << "software interrupt\n";
    ASM("movs pc, r14");
}

void _prefetch_abort() __attribute__ ((naked));
void _prefetch_abort()
{
    kout << "prefetch abort\n";
    ASM("subs pc, r14, #4");
}

void _data_abort() __attribute__ ((naked));
void _data_abort()
{
    kout << "data abort\n";
    ASM("subs pc, r14, #8");
}

void _reserved() __attribute__ ((naked));
void _reserved()
{
    kout << "reserved\n";
    ASM("mov pc, r14");
}

// TODO: Document why this mess is necessary
void _irq() __attribute__ ((naked));
void _irq() {
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

        "bl _int_dispatch                           \n"

        "ldmfd sp!, {r0}                            \n"
        // Restore IRQ's spsr value to SVC's spsr
        "msr spsr_cfxs, r0                          \n"
        // Restore context, the ^ in the end of the above instruction makes the
        // irq_spsr to be restored into svc_cpsr
        "ldmfd sp!, {r0-r3, r12, lr, pc}^           \n"
    );
}

void _fiq() __attribute__ ((naked));
void _fiq()
{
    kout << "fiq handler\n";
    ASM("subs pc, r14, #4");
}

};

__END_SYS

#endif
//__END_SYS
