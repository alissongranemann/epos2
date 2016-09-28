// EPOS ARMv7 Time-Stamp Counter Mediator Initialization

#include <tsc.h>

#ifdef __mmod_zynq__

__BEGIN_SYS

// Adapted from http://stackoverflow.com/a/3250835/3574687
void TSC::init()
{
    db<Init, TSC>(TRC) << "TSC::init()" << endl;

    // Enable all counters and reset cycle counter
    ASM("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(1<<2 || 1<<0));

    // Enable the cycle counter
    ASM("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x80000000));
}

__END_SYS

#else

#include <machine.h>
#include <ic.h>

__BEGIN_SYS

volatile TSC::Time_Stamp TSC::_overflow = 0;

void TSC::init()
{
    db<Init, TSC>(TRC) << "TSC::init()" << endl;

    reg(Machine_Model::GPTMCTL) &= ~Machine_Model::TAEN; // Disable timer
    Machine_Model::power_tsc(FULL);
    reg(Machine_Model::GPTMCFG) = 0; // 32-bit timer
    if(Traits<Build>::MODEL == Traits<Build>::LM3S811)
        reg(Machine_Model::GPTMTAMR) = 1; // One-shot
    else {
        reg(Machine_Model::GPTMTAMR) = Machine_Model::TCDIR | 2; // Up-counting, periodic

        IC::int_vector(IC::INT_TSC, int_handler);
        IC::enable(IC::INT_TSC);

        reg(Machine_Model::GPTMIMR) |= Machine_Model::TATO_INT; // Enable timeout interrupt
        reg(Machine_Model::GPTMCTL) |= Machine_Model::TAEN; // Enable timer
    }
}

__END_SYS

#endif
