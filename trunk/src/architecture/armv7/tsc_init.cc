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
    Machine_Model::power_user_timer((Traits<Build>::MODEL == Traits<Build>::eMote3) ? 3 : 1, FULL);
    reg(Machine_Model::GPTMCFG) = 0; // 32-bit timer
    if(Traits<Build>::MODEL == Traits<Build>::LM3S811)
        reg(Machine_Model::GPTMTAMR) = 1; // One-shot
    else {
        reg(Machine_Model::GPTMTAMR) = Machine_Model::TCDIR | 2; // Up-counting, periodic

        static const IC::Interrupt_Id int_id = Machine_Model::TIMERS == 1 ? IC::INT_USER_TIMER0 : Machine_Model::TIMERS == 2 ? IC::INT_USER_TIMER1 : Machine_Model::TIMERS == 3 ? IC::INT_USER_TIMER2 : IC::INT_USER_TIMER3;
        IC::int_vector(int_id, int_handler);
        IC::enable(int_id);

        reg(Machine_Model::GPTMIMR) |= Machine_Model::TATO_INT; // Enable timeout interrupt
        reg(Machine_Model::GPTMCTL) |= Machine_Model::TAEN; // Enable timer
    }
}

__END_SYS

#endif
