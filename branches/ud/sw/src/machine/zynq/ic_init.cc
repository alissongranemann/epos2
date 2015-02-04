// EPOS Zynq Interrupt Controller Initialization

#include <cpu.h>
#include <ic.h>
#include <machine.h>

__BEGIN_SYS

void PandaBoard_IC::init()
{
    db<Init, IC>(TRC) << "IC::init()" << endl;

    CPU::int_disable();

    if(Machine::cpu_id() == 0)
        interrupt_distributor_init();
    //interrupt_interface_init();

    CPU::int_enable();
}

void Zynq_IC::init()
{
    db<Init, IC>(TRC) << "IC::init()" << endl;

    CPU::int_disable();

    if(Machine::cpu_id() == 0)
        interrupt_distributor_init();
    CPU::int_enable();

    CPU::int_enable();
}

__END_SYS
