// EPOS ARMv7 CPU Mediator Implementation

#include <architecture/armv7/cpu.h>
#include <system/config.h>
#include <machine.h>

__BEGIN_SYS

ARMv7::OP_Mode ARMv7::_mode = ARMv7::FULL;
bool ARMv7::_int_enabled = false;

void ARMv7::Context::save() volatile
{
	ASM("nop\n");
}

void ARMv7::Context::load() const volatile
{
	db<CPU>(TRC) << "CPU::Context::load(this=" << (void*)this << ")\n";

	ASM("ldr r0, [%0, #64]\n"
         "msr spsr_cfsx, r0\n"
         "ldmfd %0, {r0-r12,sp,lr,pc}^\n"
		 :
		 : "r" (this)
		 : "r0");
}


void ARMv7::switch_context(Context * volatile * o, Context * volatile n)
{
	kout << "Switch_context\n";
    Context * old = *o;

    old->_cpsr = CPU::flags();

    //ASM("ldr r2, [%0, #64]" : : "r"(n) : "r2"); //geting n->_cpsr
    //ASM("msr spsr_cfsx, r2");

    ASM("msr spsr_cfsx, %0" : : "r"(n->_cpsr) :);
    ASM("stmia %0, {r0-r12,sp,lr,pc} \n"              // pc is always read with a +8 offset
         "ldmfd %1, {r0-r12,sp,lr,pc}^"
          : : "r"(old), "r"(n) :);
    ASM("nop");                                        // so the pc read above is poiting here
}


void ARMv7::power(ARMv7::OP_Mode mode)
{
    if (mode == _mode) return;
        _mode = mode;
}

extern "C" void __cxa_guard_acquire() {
    CPU::int_disable();
}
extern "C" void __cxa_guard_release() {
    CPU::int_enable();
}

__END_SYS

