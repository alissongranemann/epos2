// EPOS Component Framework

#ifndef __framework_h
#define __framework_h

#include <utility/hash.h>

__BEGIN_SYS

class Framework
{
    template<typename> friend class Handled;
    template<typename> friend class Proxied;

private:
    typedef Simple_Hash<void, 5, unsigned int> Cache; // TODO: a real cache, with operator >> instead of % would improve performance
    typedef Cache::Element Element;

public:
    Framework() {}

private:
    static Cache _cache;
};

__END_SYS

#include <cpu.h>
#include <mmu.h>
#include <system.h>
#include <thread.h>
#include <task.h>
#include <alarm.h>
#include <address_space.h>
#include <segment.h>
#include <mutex.h>
#include <semaphore.h>
#include <condition.h>
#include <communicator.h>

#include "handle.h"

#define BIND(X) typedef _SYS::IF<(_SYS::Traits<_SYS::X>::ASPECTS::Length || (_SYS::Traits<_SYS::Build>::MODE == _SYS::Traits<_SYS::Build>::KERNEL)), _SYS::Handle<_SYS::X>, _SYS::X>::Result X;
#define EXPORT(X) typedef _SYS::X X;

#define SELECT(X) _SYS::IF<(_SYS::Traits<_SYS::X>::ASPECTS::Length || (_SYS::Traits<_SYS::Build>::MODE == _SYS::Traits<_SYS::Build>::KERNEL)), _SYS::Handle<_SYS::X>, _SYS::X>::Result

__BEGIN_API

__USING_UTIL

EXPORT(CPU);
EXPORT(Handler);
EXPORT(Function_Handler);
// EXPORT(Semaphore_Handler);
class Semaphore_Handler: public _SYS::Handler
{
public:
    Semaphore_Handler(_SYS::Handle<_SYS::Semaphore> * h) : _handler(h) {}
    ~Semaphore_Handler() {}

    void operator()() { _handler->v(); }

private:
    _SYS::Handle<_SYS::Semaphore> * _handler;
};

// EXPORT(Thread_Handler);
class Thread_Handler: public _SYS::Handler
{
public:
    Thread_Handler(_SYS::Handle<_SYS::Thread> * h) : _handler(h) {}
    ~Thread_Handler() {}

    void operator()() { _handler->resume(); }

private:
    _SYS::Handle<_SYS::Thread> * _handler;
};

// EXPORT(Condition_Handler);
class Condition_Handler: public _SYS::Handler
{
public:
    Condition_Handler(_SYS::Handle<_SYS::Condition> * h) : _handler(h) {}
    ~Condition_Handler() {}

    void operator()() { _handler->signal(); }

private:
    _SYS::Handle<_SYS::Condition> * _handler;
};


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
// BIND(Alarm);
class Alarm: public SELECT(Alarm)
{
    typedef SELECT(Alarm) Base;
public:
    Alarm(const _SYS::RTC::Microsecond & time, _SYS::Handler * handler, int times = 1): Base(time, handler, times) {}

    static _SYS::TSC::Hertz frequency() {
        return Base::alarm_frequency();
    }


};
BIND(Delay);

BIND(Network);
EXPORT(IPC);
EXPORT(IP);
EXPORT(ICMP);
EXPORT(UDP);
EXPORT(TCP);
EXPORT(DHCP);

template<typename Channel, bool connectionless = Channel::connectionless>
class Link: public _SYS::Handle<_SYS::Link<Channel, connectionless>>
{
private:
    typedef typename _SYS::Handle<_SYS::Link<Channel, connectionless>> Base;

public:
    template<typename ... Tn>
    Link(const Tn & ... an): Base(an ...) {};
};

template<typename Channel, bool connectionless = Channel::connectionless>
class Port: public _SYS::Handle<_SYS::Port<Channel, connectionless>>
{
private:
    typedef typename _SYS::Handle<_SYS::Port<Channel, connectionless>> Base;

public:
    template<typename ... Tn>
    Port(const Tn & ... an): Base(an ...) {};
};

__END_API

#endif
