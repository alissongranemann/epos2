// This is a temporary file to develop the Cortex_M3 Port

#include <machine.h>

__BEGIN_SYS

void _int_enable() {
    ASM("mov r0, #1 \n"
        "msr primask, r0 \n"
        "msr faultmask, r0");
//    ASM("cpsie f");
    Cortex_Model_Specifics::scs(Cortex_Model_Specifics::STCTRL) |= Cortex_Model_Specifics::INTEN;
}

void _int_disable() {
    Cortex_Model_Specifics::scs(Cortex_Model_Specifics::STCTRL) &= ~Cortex_Model_Specifics::INTEN;
//    ASM("cpsid f");
    ASM("mov r0, #0 \n"
        "msr primask, r0 \n"
        "msr faultmask, r0");
}

__END_SYS
