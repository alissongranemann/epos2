// EPOS ARMV7 Time-Stamp Counter Mediator Initialization

#include <tsc.h>

__BEGIN_SYS

// Adapted from http://stackoverflow.com/a/3250835/3574687
void ARMV7_TSC::init()
{
    db<Init, ARMV7_TSC>(TRC) << "ARMV7_TSC::init()" << endl;

    // Enable all counters and reset cycle counter
    ASMV("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(1<<2 || 1<<0));

    // Enable the cycle counter
    ASMV("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x80000000));
}

__END_SYS
