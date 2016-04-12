#ifndef __traits_h
#define __traits_h

#include <system/config.h>

__BEGIN_SYS

// Global Configuration
template<typename T>
struct Traits
{
    static const bool enabled = true;
    static const bool debugged = true;
    static const bool hysterically_debugged = false;
    typedef TLIST<> ASPECTS;
};

template<> struct Traits<Build>
{
    enum {LIBRARY, BUILTIN, KERNEL};
    static const unsigned int MODE = KERNEL;

    enum {IA32};
    static const unsigned int ARCHITECTURE = IA32;

    enum {PC};
    static const unsigned int MACHINE = PC;

    enum {Legacy};
    static const unsigned int MODEL = Legacy;

    static const unsigned int CPUS = 3;
    static const unsigned int NODES = 2; // > 1 => NETWORKING

    static const bool NO_LOADER = false;
    static const bool NO_DOMAIN_2 = false;

    static const bool RUNNING_ON_QEMU = false;
};

template<> struct Traits<Domain_1>
{
    static const bool debugged = true;
    static const bool DBLV0 = true;
    static const bool DBLV1 = true;
    static const bool DBLV2 = false;
};

template<> struct Traits<Domain_2>
{
    static const bool debugged = true;
    static const bool DBLV0 = true;
    static const bool DBLV1 = true;
    static const bool DBLV2 = false;
};


// Utilities
template<> struct Traits<Debug>
{
    static const bool error   = true;
    static const bool warning = true;
    static const bool info    = false;
    static const bool trace   = false;
};

template<> struct Traits<Lists>: public Traits<void>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Spin>: public Traits<void>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Heaps>: public Traits<void>
{
    static const bool debugged = true;
};


// System Parts (mostly to fine control debugging)
template<> struct Traits<Boot>: public Traits<void>
{
};

template<> struct Traits<Setup>: public Traits<void>
{
};

template<> struct Traits<Init>: public Traits<void>
{
};

template<> struct Traits<Framework>: public Traits<void>
{
};

template<> struct Traits<Aspect>: public Traits<void>
{
    static const bool debugged = hysterically_debugged;
};

// Mediators
template<> struct Traits<Serial_Display>: public Traits<void>
{
    static const bool enabled = false;
    static const int COLUMNS = 80;
    static const int LINES = 24;
    static const int TAB_SIZE = 8;
};

__END_SYS

#include __ARCH_TRAITS_H
#include __MACH_CONFIG_H
#include __MACH_TRAITS_H

__BEGIN_SYS


// Abstractions
template<> struct Traits<Application>: public Traits<void>
{
    static const unsigned int STACK_SIZE = Traits<Machine>::STACK_SIZE;
    static const unsigned int HEAP_SIZE = Traits<Machine>::HEAP_SIZE;
    static const unsigned int MAX_THREADS = Traits<Machine>::MAX_THREADS;
    static const unsigned long DOMAIN_PERIOD = 5 * 1000 * 1000; // seconds

    static const unsigned long MAX_INTERF = 1000 * 1;   // 1 ms
    static const unsigned long MED_INTERF = 1000 * 10;  // 10 ms
    static const unsigned long MIN_INTERF = 1000 * 100; // 100 ms
    static const unsigned long MIN_MIN_INTERF = 1000 * 500; // 500 ms
    static const unsigned long LESS_THAN_MIN_INTERF = 1000 * 1000; // 1s

    static const unsigned long TIME_BETWEEN_SEGMENTS = LESS_THAN_MIN_INTERF; /*! Experiment Factor Set in Client */

    static const unsigned long GUEST_OS_1_TASK_ITERATIONS = 3;
    static const unsigned long GUEST_OS_TASK_ITERATIONS = GUEST_OS_1_TASK_ITERATIONS;
    static const unsigned long GUEST_OS_2_TASK_ITERATIONS = (GUEST_OS_1_TASK_ITERATIONS * DOMAIN_PERIOD) / TIME_BETWEEN_SEGMENTS;
    static const unsigned int NUM_OF_DOMAINS = 3;
    static const bool IS_DOM_1_HRT = true;
    static const bool IS_DOM_2_HRT = false;

    // static const bool DONT_RUN_GUEST_OS_2_TASK = Traits<Build>::NODES <= 1;
    static const bool DONT_RUN_GUEST_OS_2_TASK = false;
};

template<> struct Traits<System>: public Traits<void>
{
    static const unsigned int mode = Traits<Build>::MODE;
    static const bool multithread = (Traits<Application>::MAX_THREADS > 1);
    static const bool multitask = (mode != Traits<Build>::LIBRARY);
    static const bool multicore = true;
    static const bool multiheap = (mode != Traits<Build>::LIBRARY) || Traits<Scratchpad>::enabled;

    enum {FOREVER = 0, SECOND = 1, MINUTE = 60, HOUR = 3600, DAY = 86400, WEEK = 604800, MONTH = 2592000, YEAR = 31536000};
    static const unsigned long LIFE_SPAN = 1 * HOUR; // in seconds

    static const bool reboot = true;

    static const unsigned int STACK_SIZE = Traits<Machine>::STACK_SIZE;
    static const unsigned int HEAP_SIZE = (Traits<Application>::MAX_THREADS + 1) * Traits<Application>::STACK_SIZE;
};

template<> struct Traits<Task>: public Traits<void>
{
    static const bool enabled = Traits<System>::multitask;
};

template<> struct Traits<Thread>: public Traits<void>
{
    static const bool smp = Traits<System>::multicore;

    typedef Scheduling_Criteria::PEDF Criterion;
    static const unsigned int QUANTUM = 10000; // us

    static const bool trace_idle = hysterically_debugged;
};

template<> struct Traits<Scheduler<Thread> >: public Traits<void>
{
    static const bool debugged = Traits<Thread>::trace_idle || hysterically_debugged;
};

template<> struct Traits<Periodic_Thread>: public Traits<void>
{
    static const bool simulate_capacity = false;
};

template<> struct Traits<Address_Space>: public Traits<void>
{
    static const bool enabled = Traits<System>::multiheap;
};

template<> struct Traits<Segment>: public Traits<void>
{
    static const bool enabled = Traits<System>::multiheap;
};

template<> struct Traits<Alarm>: public Traits<void>
{
    static const bool visible = hysterically_debugged;
};

template<> struct Traits<Synchronizer>: public Traits<void>
{
    static const bool enabled = Traits<System>::multithread;
};

template<> struct Traits<Network>: public Traits<void>
{
    static const bool enabled = (Traits<Build>::NODES > 1);

    static const unsigned int RETRIES = 3;
    static const unsigned int TIMEOUT = 10; // s

    // This list is positional, with one network for each NIC in traits<NIC>::NICS
    typedef LIST<IP> NETWORKS;
};

template<> struct Traits<IP>: public Traits<Network>
{
    enum {STATIC, MAC, INFO, RARP, DHCP};

    struct Default_Config {
        static const unsigned int  TYPE    = DHCP;
        static const unsigned long ADDRESS = 0;
        static const unsigned long NETMASK = 0;
        static const unsigned long GATEWAY = 0;
    };

    template<unsigned int UNIT>
    struct Config: public Default_Config {};

    static const unsigned int TTL  = 0x40; // Time-to-live
};

template<> struct Traits<IP>::Config<0> //: public Traits<IP>::Default_Config
{
    static const unsigned int  TYPE      = MAC;
    static const unsigned long ADDRESS   = 0xc2a70100;  // 194.167.1.x x=MAC[5]
    // static const unsigned long ADDRESS   = 0x0a000100;  // 10.0.1.x x=MAC[5]
    static const unsigned long NETMASK   = 0xffffff00;  // 255.255.255.0
    static const unsigned long GATEWAY   = 0;           // 10.0.1.1
};

template<> struct Traits<IP>::Config<1>: public Traits<IP>::Default_Config
{
};

template<> struct Traits<UDP>: public Traits<Network>
{
    static const bool checksum = true;
};

template<> struct Traits<TCP>: public Traits<Network>
{
    static const unsigned int WINDOW = 4096;
};

template<> struct Traits<DHCP>: public Traits<Network>
{
};

__END_SYS


#endif
