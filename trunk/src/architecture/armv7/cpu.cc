// EPOS ARMv7 CPU Mediator Implementation

#include <architecture/armv7/cpu.h>
#include <system.h>

__BEGIN_SYS

// Class attributes
unsigned int CPU::_cpu_clock;
unsigned int CPU::_bus_clock;

// Class methods
void CPU::Context::save() volatile
{
    ASM("       mov     r12, pc                 \n"
        "       push    {r12}                   \n"
        "       push    {r0-r12, lr}            \n");
    mrs12();
    ASM("       push    {r12}                   \n"
        "       str     sp, [%0]                \n"
        : : "r"(this));
}

void CPU::Context::load() const volatile
{
    System::_heap->free(reinterpret_cast<void *>(Memory_Map::SYS_STACK), Traits<System>::STACK_SIZE);
    ASM("       mov     sp, %0                  \n"
        "       isb                             \n"     // serialize the pipeline so that SP gets updated before the pop
        "       pop     {r12}                   \n" : : "r"(this));
    msr12();
    ASM("       pop     {r0-r12, lr}            \n"
        "       pop     {pc}                    \n");
}

void CPU::switch_context(Context * volatile * o, Context * volatile n)
{
    ASM("       adr     r12, .ret               \n"
        "       push    {r12}                   \n"
        "       push    {r0-r12, lr}            \n");
    mrs12();
    ASM("       push    {r12}                   \n"
        "       str     sp, [%0]                \n"
        "       mov     sp, %1                  \n"
        "       isb                             \n"     // serialize the pipeline so that SP gets updated before the pop
        "       pop     {r12}                   \n" : : "r"(o), "r"(n));
    msr12();
    ASM("       pop     {r0-r12, lr}            \n"
        "       pop     {r12}                   \n"
        "       mov     pc, r12                 \n"     // popping directly into PC causes an Usage Fault???
        ".ret:  bx      lr                      \n");
}

__END_SYS
