#include <ic.h>

__BEGIN_SYS

extern "C" {

void _undefined_instruction() __attribute__ ((naked));
void _undefined_instruction()
{
    kout << "undefined instruction\n";
    while(1);
}

void _software_interrupt() __attribute__ ((naked));
void _software_interrupt()
{
    kout << "software interrupt\n";
    while(1);
}

void _prefetch_abort() __attribute__ ((naked));
void _prefetch_abort()
{
    kout << "prefetch abort\n";
    while(1);
}

void _data_abort() __attribute__ ((naked));
void _data_abort()
{
    kout << "data abort\n";
    while(1);
}

void _reserved() __attribute__ ((naked));
void _reserved()
{
    kout << "reserved\n";
    while(1);
}

void _fiq() __attribute__ ((naked));
void _fiq()
{
    kout << "fiq handler\n";
    while(1);
}

};

__END_SYS
