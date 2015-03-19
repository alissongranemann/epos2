// EPOS Component Framework

#ifndef __framework_h
#define __framework_h

#include <cpu.h>
#include <mmu.h>
#include <task.h>
#include <thread.h>
#include <system.h>
#include <alarm.h>
#include <machine.h>

#include "handle.h"

#define BIND(X) typedef _SYS::IF<(_SYS::Traits<_SYS::X>::ASPECTS::Length || (_SYS::Traits<_SYS::Build>::MODE == _SYS::Traits<_SYS::Build>::KERNEL)), _SYS::Handle<_SYS::X>, _SYS::X>::Result X;
#define EXPORT(X) typedef _SYS::X X;

__BEGIN_API

__USING_UTIL

EXPORT(Display);
EXPORT(CPU);
EXPORT(Handler);
EXPORT(Function_Handler);
// typedef _SYS::Functor_Handler Functor_Handler;
EXPORT(Concurrent_Observed);
EXPORT(Concurrent_Observer);
EXPORT(User_Timer); // It should be a BIND, or?

EXPORT(System);
EXPORT(Application);
BIND(Thread);
BIND(Active);
BIND(Periodic_Thread);
BIND(RT_Thread);
BIND(Task);
BIND(Address_Space);
BIND(Segment);
BIND(Mutex);
BIND(Semaphore);
BIND(Condition);
BIND(Clock);
BIND(Chronometer);
BIND(Alarm);
BIND(Delay);
BIND(Boot_Image);
// BIND(ELF);
typedef _SYS::Handle<_SYS::U::ELF> User_Space_ELF;

__END_API

#endif
