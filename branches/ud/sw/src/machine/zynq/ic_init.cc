// EPOS Zynq Interrupt Controller Initialization

#include <cpu.h>
#include <ic.h>
#include <machine.h>

__BEGIN_SYS

void Zynq_IC::init()
{
    db<Init, IC>(TRC) << "IC::init()" << endl;

    CPU::int_disable(); // Will be reenabled at Thread::init()
    interrupt_distributor_init();
    disable(); // Will be enabled on demand as handlers are registered

    // Set all interrupt handlers to int_not()
    for(Interrupt_Id i = 0; i < INTS; i++)
        _int_vector[i] = int_not;
}

__END_SYS
