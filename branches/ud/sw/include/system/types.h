// EPOS Internal Type Management System

typedef __SIZE_TYPE__ size_t;

#ifndef __types_h
#define __types_h

__BEGIN_SYS

// Memory allocators
enum System_Allocator
{
    SYSTEM
};

enum Scratchpad_Allocator
{
    SCRATCHPAD
};

__END_SYS

extern "C"
{
    void * malloc(size_t);
    void free(void *);
}

inline void * operator new(size_t s, void * a) { return a; }
inline void * operator new[](size_t s, void * a) { return a; }

void * operator new(size_t, const EPOS::System_Allocator &);
void * operator new[](size_t, const EPOS::System_Allocator &);

void * operator new(size_t, const EPOS::Scratchpad_Allocator &);
void * operator new[](size_t, const EPOS::Scratchpad_Allocator &);

#include "../../../unified/framework/types.h"

__BEGIN_SYS

// Dummy class for incomplete architectures and machines
template<int>
class Dummy;

// Utilities
class Debug;
class Lists;
class Spin;
class Heap;

// System parts
class Build;
class Boot;
class Setup;
class Init;
class System;
class Application;

// Hardware Mediators - CPU
class IA32;
class ARMV7;
class MIPS32;

// Hardware Mediators - Time-Stamp Counter
class IA32_TSC;
class ARMV7_TSC;
class MIPS32_TSC;

// Hardware Mediators - Memory Management Unit
class IA32_MMU;
class ARMV7_MMU;
class MIPS32_MMU;

// Hardware Mediators - Performance Monitoring Unit
class IA32_PMU;

// Hardware Mediators - Machine
class PC;
class Zynq;
class EPOSSOC;

// Hardware Mediators - Busses
class PC_PCI;
class EPOSSOC_NOC;

// Hardware Mediators - Interrupt Controller
class PC_IC;
class Zynq_IC;
class EPOSSOC_IC;

// Hardware Mediators - Timer
class PC_Timer;
class Zynq_Timer;
class EPOSSOC_Timer;

// Hardware Mediators - Real-time Clock
class PC_RTC;
class Zynq_RTC;
class EPOSSOC_RTC;

// Hardware Mediators - EEPROM
class PC_EEPROM;

// Hardware Mediators - Scratchpad
class PC_Scratchpad;

// Hardware Mediators - UART
class PC_UART;
class Zynq_UART;
class EPOSSOC_UART;

// Hardware Mediators - GPIO
class Zynq_GPIO;

// Hardware Mediators - Display
class Serial_Display;
class PC_Display;

// Hardware Mediators - NIC
class PC_Ethernet;
class PCNet32;
class C905;
class E100;
class Zynq_NIC;
class EPOSSOC_NIC;

// Hardware Mediators - Component Controller
class Zynq_Component_Controller;
class EPOSSOC_Component_Controller;

// Hardware Mediators - PCAP
class Zynq_PCAP;
class EPOSSOC_PCAP;

// Abstractions	- Process
class Thread;
class Active;
class Periodic_Thread;
class RT_Thread;
class Task;

// Abstractions - Scheduler
template <typename> class Scheduler;
namespace Scheduling_Criteria
{
    class Priority;
    class FCFS;
    class RR;
    class RM;
    class DM;
    class EDF;
    class CPU_Affinity;
    class GEDF;
    class PEDF;
    class CEDF;
};

// Abstractions	- Memory
class Segment;
class Address_Space;

// Abstractions	- Synchronization
class Synchronizer;
class Mutex;
class Semaphore;
class Condition;

// Abstractions	- Time
class Clock;
class Alarm;
class Chronometer;

// Abstractions - Network
class Ethernet;
template<typename NIC, typename Network, unsigned int HTYPE = 1>
class ARP;
class Network;
class IP;
class ICMP;
class UDP;
class TCP;
class DHCP;

// Abstractions - Component Manager
class Component_Manager;

// System Components IDs
// The order in this enumeration defines many things in the system (e.g. init)
typedef unsigned int Type_Id;
enum
{
    CPU_ID,
    TSC_ID,
    MMU_ID,

    MACHINE_ID,
    PCI_ID,
    NOC_ID,
    IC_ID,
    TIMER_ID,
    RTC_ID,
    EEPROM_ID,
    SCRATCHPAD_ID,
    UART_ID,
    DISPLAY_ID,
    NIC_ID,
    COMPONENT_CONTROLLER_ID,

    THREAD_ID,
    TASK_ID,
    ACTIVE_ID,

    SEGMENT_ID,
    ADDRESS_SPACE_ID,

    MUTEX_ID,
    SEMAPHORE_ID,
    CONDITION_ID,

    CLOCK_ID,
    ALARM_ID,
    CHRONOMETER_ID,

    COMPONENT_MANAGER_ID,

    UNKNOWN_TYPE_ID,
    LAST_TYPE_ID = UNKNOWN_TYPE_ID - 1
};

__END_SYS

#endif
