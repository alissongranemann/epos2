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
    static const unsigned int MODE = LIBRARY;

    enum {IA32, ARMv7};
    static const unsigned int ARCHITECTURE = ARMv7;

    enum {PC, Cortex_M, Cortex_A};
    static const unsigned int MACHINE = Cortex_M;

    enum {Legacy, eMote3, LM3S811};
    static const unsigned int MODEL = LM3S811;

    static const unsigned int CPUS = 1;
    static const unsigned int NODES = 1; // > 1 => NETWORKING
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
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Observers>: public Traits<void>
{
    // Some observed objects are created before initializing the Display
    // Enabling debug may cause trouble in some Machines
    static const bool debugged = false;

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


// Mediators
template<> struct Traits<Serial_Display>: public Traits<void>
{
    static const bool enabled = true;
    static const int COLUMNS = 80;
    static const int LINES = 24;
    static const int TAB_SIZE = 8;
};

template<> struct Traits<Serial_Keyboard>: public Traits<void>
{
    static const bool enabled = false;
};

__END_SYS

#include __ARCH_TRAITS_H
#include __MACH_TRAITS_H
#include __MACH_CONFIG_H

__BEGIN_SYS


// Abstractions
template<> struct Traits<Application>: public Traits<void>
{
    static const unsigned int STACK_SIZE = Traits<Machine>::STACK_SIZE;
    static const unsigned int HEAP_SIZE = Traits<Machine>::HEAP_SIZE;
    static const unsigned int MAX_THREADS = Traits<Machine>::MAX_THREADS;
};

template<> struct Traits<System>: public Traits<void>
{
    static const unsigned int mode = Traits<Build>::MODE;
    static const bool multithread = (Traits<Application>::MAX_THREADS > 1);
    static const bool multitask = (mode != Traits<Build>::LIBRARY);
    static const bool multicore = (Traits<Build>::CPUS > 1) && multithread;
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

    typedef Scheduling_Criteria::RR Criterion;
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
    typedef LIST<TSTPOE> NETWORKS;
};

template<> struct Traits<TSTP>: public Traits<Network>
{
    static const bool debugged = true;
    //static const bool is_sink = false;


    // MAC component selection. Possible values = {TSTP_MAC, One_Hop_MAC};
    typedef TSTPOE MAC;
    
    // Time management component selection. Possible values = {PTS};
    typedef PTS Time_Manager;

    // Locator component selection. Possible values = {NIC_Locator, Static_Locator};
    typedef NIC_Locator Locator;

    // Router component selection. Possible values = {SGGR};
    //typedef SGGR Router;

    // Security component selection. Possible values = {TSTP_Security};
    //typedef TSTP_Security Security;


    /*
    // Default configurations that apply to all MACs
    template<typename MAC>
    struct MAC_Config_App {
        static const unsigned int RECEIVE_BUFFERS = 4;
        static const unsigned int SEND_BUFFERS = 64 - RECEIVE_BUFFERS;
    };
    // Machine- and unit-dependent MAC configurations, definable in machine traits 
    template<unsigned int unit = 0, typename MAC = Traits<TSTP>::MAC>
    struct MAC_Config : public Traits<MAC> {};

    // Default configurations that apply to all routers
    template<typename ROUTER>
    struct Router_Config_App {};
    // Machine- and unit-dependent router configurations, definable in machine traits 
    template<unsigned int unit = 0, typename ROUTER = Traits<TSTP>::Router>
    struct Router_Config : public Traits<ROUTER> {};

    // Default configurations that apply to all time managers
    template<typename TIME_MANAGER>
    struct Time_Config_App {};
    // Machine- and unit-dependent time manager configurations, definable in machine traits 
    template<unsigned int unit = 0, typename TIME_MANAGER = Traits<TSTP>::Time_Manager>
    struct Time_Config : public Traits<TIME_MANAGER> {};
    */
};

template<> struct Traits<Static_Locator>: public Traits<TSTP>
{
    static const unsigned int X = 10;
    static const unsigned int Y = 10;
    static const unsigned int Z = 10;
};

template<> template <typename S> struct Traits<Smart_Data<S>>: public Traits<Network>
{
    static const bool debugged = true;
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
    static const unsigned long ADDRESS   = 0x0a000100;  // 10.0.1.x x=MAC[5]
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