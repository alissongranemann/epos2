#include <machine.h>
#include <ic.h>

__BEGIN_SYS

// FIXME: incorporate in IC as methods and alias them at Setup.S
extern "C" {

void _undefined_instruction() __attribute__ ((naked));
void _undefined_instruction()
{
    db<CPU>(ERR) << "Undefined instruction" << endl;
    Machine::panic();
}

void _software_interrupt() __attribute__ ((naked));
void _software_interrupt()
{
    db<CPU>(ERR) << "Software interrupt" << endl;
    Machine::panic();
}

void _prefetch_abort() __attribute__ ((naked));
void _prefetch_abort()
{
    db<CPU>(ERR) << "Prefetch abort" << endl;
    Machine::panic();
}

void _data_abort() __attribute__ ((naked));
void _data_abort()
{
    db<CPU>(ERR) << "Data abort" << endl;
    Machine::panic();
}

void _reserved() __attribute__ ((naked));
void _reserved()
{
    db<CPU>(ERR) << "Reserved" << endl;
    Machine::panic();
}

void _fiq() __attribute__ ((naked));
void _fiq()
{
    db<CPU>(ERR) << "FIQ handler" << endl;
    Machine::panic();
}

};

__END_SYS
