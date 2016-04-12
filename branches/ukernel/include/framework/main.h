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


#include <machine.h>
#include <rtc.h>
#include <cpu.h>
#include <mmu.h>
#include <mmu_aux.h>
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
#include <periodic_thread.h>
#include <ip.h>
#include <tcp.h>
#include <chronometer.h>

#include "handle.h"
#include "proxy.h"

#define BIND(X) typedef _SYS::IF<(_SYS::Traits<_SYS::X>::ASPECTS::Length || (_SYS::Traits<_SYS::Build>::MODE == _SYS::Traits<_SYS::Build>::KERNEL)), _SYS::Handle<_SYS::X>, _SYS::X>::Result X;

#define EXPORT(X) typedef _SYS::X X;

#define SELECT(X) _SYS::IF<(_SYS::Traits<_SYS::X>::ASPECTS::Length || (_SYS::Traits<_SYS::Build>::MODE == _SYS::Traits<_SYS::Build>::KERNEL)), _SYS::Handle<_SYS::X>, _SYS::X>::Result



__BEGIN_API

__USING_UTIL

BIND(Machine);

// BIND(CPU);
class CPU: public SELECT(CPU)
{
private:
    typedef SELECT(CPU) Base;
    typedef _SYS::CPU Sys;

public:
    typedef Sys::Log_Addr Log_Addr;

};

BIND(MMU_Aux);

EXPORT(Handler);
EXPORT(Function_Handler);
EXPORT(RTC);

// BIND(TSC);
class TSC: public SELECT(TSC)
{
private:
    typedef SELECT(TSC) Base;
    typedef _SYS::TSC Sys;

public:
    typedef Sys::Time_Stamp Time_Stamp; /* unsigned long long */

};

EXPORT(System);
EXPORT(Application);

BIND(Active);
class Task;
// BIND(Task);
class Task: public SELECT(Task)
{
private:
    typedef SELECT(Task) Base;

public:
    template<typename ... Tn>
    Task(void * cs, void * ds, int (* entry)(Tn ...), Tn ... an)
    :Base(reinterpret_cast<SELECT(Segment) *>(cs)->__stub()->id().unit(),
            reinterpret_cast<SELECT(Segment) *>(ds)->__stub()->id().unit(),
            entry,
            an ...
    )
    {
    }

    template<typename ... Tn>
    Task(void * thread, void * cs, void * ds, int (* entry)(Tn ...))
    :Base(thread ? reinterpret_cast<SELECT(Thread) *>(thread)->__stub()->id().unit() : 0,
            reinterpret_cast<SELECT(Segment) *>(cs)->__stub()->id().unit(),
            reinterpret_cast<SELECT(Segment) *>(ds)->__stub()->id().unit(),
            entry
    )
    {
    }

    static Task * volatile self()
    {
        return reinterpret_cast<Task *>(Base::self());
    }

};

BIND(Address_Space);
BIND(Segment);

BIND(Mutex);
BIND(Semaphore);
BIND(Condition);

BIND(Clock);
BIND(Chronometer);
BIND(Chronometer_Aux);
BIND(Alarm);
BIND(Delay);

BIND(ICMP);
BIND(UDP);
BIND(DHCP);
EXPORT(IPC);

BIND(This_Thread);

namespace Scheduling_Criteria
{
// BIND(PEDF);
class PEDF: public SELECT(Scheduling_Criteria::PEDF)
{
private:
    typedef SELECT(Scheduling_Criteria::PEDF) Base;
    typedef _SYS::Scheduling_Criteria::PEDF Sys;

public:
    typedef RTC::Microsecond Microsecond;


public:
    enum {
        MAIN   = Sys::MAIN,
        HIGH   = Sys::HIGH,
        NORMAL = Sys::NORMAL,
        LOW    = Sys::LOW,
        IDLE   = Sys::IDLE
    };

    enum {
        PERIODIC    = Sys::PERIODIC,
        APERIODIC   = Sys::APERIODIC
    };

    // Constructor helpers
    enum {
        SAME        = Sys::SAME,
        NOW         = Sys::NOW,
        UNKNOWN     = Sys::UNKNOWN,
        INFINITE    = Sys::INFINITE
    };

public:
    // operator const volatile int() const volatile { return 0; /* TODO */ }

    friend Debug & operator<<(Debug & db, const PEDF & pedf)
    {
        print_pedf(&(reinterpret_cast<Base &>(const_cast<PEDF &>(pedf))));
        return db;
    }

    static void print_pedf(Base * pedf)
    {
        // db<void>(WRN) << "print_pedf, p = " << reinterpret_cast<void *>(pedf->__stub()->id().unit()) << endl;

        _SYS::Message msg(_SYS::Id(_SYS::PEDF_ID, 0), _SYS::Message::PRINT, pedf->__stub()->id().unit());
        msg.act();
    }

public:
    PEDF(int p = APERIODIC) : Base(p)
    {}

    PEDF(const Microsecond & d, const Microsecond & p, const Microsecond & c, int cpu) : Base(d, p, c, cpu)
    {}

    PEDF(int priority, int cpu) : Base(priority, cpu)
    {}
};
}

// BIND(Thread);
class Thread: public SELECT(Thread)
{
private:
    typedef SELECT(Thread) Base;
    typedef _SYS::Thread Sys;

protected:
    static const unsigned int STACK_SIZE = Sys::STACK_SIZE;

public:
    // Thread State
    enum State {
        RUNNING = Sys::RUNNING,
        READY = Sys::READY,
        SUSPENDED = Sys::SUSPENDED,
        WAITING = Sys::WAITING,
        FINISHING = Sys::FINISHING
    };


    // Thread Scheduling Criterion
    typedef Sys::Criterion Sys_Criterion;

    typedef _SYS::SWITCH<_SYS::Type<Sys_Criterion>::ID,
                            _SYS::CASE<_SYS::PEDF_ID, Scheduling_Criteria::PEDF,
                            _SYS::CASE<_SYS::DEFAULT, void > > // The default case will generate a compilation error
                        >::Result Criterion;

    enum {
        HIGH    = Criterion::HIGH,
        NORMAL  = Criterion::NORMAL,
        LOW     = Criterion::LOW,
        MAIN    = Criterion::MAIN,
        IDLE    = Criterion::IDLE
    };


public:
    // typedef Sys::Configuration Configuration;
    struct Configuration: public SELECT(Thread::Configuration)
    {
    private:
        typedef SELECT(Thread::Configuration) Base;
    public:
        Configuration(const State & s = READY, const Criterion & c = NORMAL, Task * t = 0, unsigned int ss = STACK_SIZE)
        : Base(t ? reinterpret_cast<_SYS::Task *>(reinterpret_cast<_SYS::Proxy<_SYS::Task> *>(t->__stub())->id().unit()) : 0
                , (Thread::Sys::State &) s
                , ss
                , reinterpret_cast<Sys_Criterion &>(const_cast<_SYS::Type_Id &>(const_cast<Criterion &>(c).__stub()->id().unit()))
              ) /* For some reason if criterion is not the last,
                 * everything that goes after criterion won't
                 * (des)serialize correctly. */
        {
        }
    };

public:
    template<typename ... Tn>
    Thread(int (* entry)(Tn ...), Tn ... an) : Base(entry, an ...)
    {
    }

    template<typename ... Tn>
    Thread(const Configuration & conf, int (* entry)(Tn ...), Tn ... an)
    : Base(const_cast<Configuration &>(conf).__stub()->id().unit()
            , entry
            , an ...)
    {
    }

};

// BIND(Periodic_Thread);
class Periodic_Thread: public SELECT(Periodic_Thread)
{
private:
    typedef SELECT(Periodic_Thread) Base;
    typedef _SYS::Periodic_Thread Sys;

protected:
    static const unsigned int STACK_SIZE = Sys::STACK_SIZE;

public:
    // Thread State
    enum State {
        RUNNING = Sys::RUNNING,
        READY = Sys::READY,
        SUSPENDED = Sys::SUSPENDED,
        WAITING = Sys::WAITING,
        FINISHING = Sys::FINISHING
    };

    // Thread Scheduling Criterion
    typedef Sys::Criterion Sys_Criterion;

    typedef Thread::Criterion Criterion;

    enum {
        HIGH    = Criterion::HIGH,
        NORMAL  = Criterion::NORMAL,
        LOW     = Criterion::LOW,
        MAIN    = Criterion::MAIN,
        IDLE    = Criterion::IDLE
    };

public:
    typedef Sys::Microsecond Microsecond;

    enum { INFINITE = Sys::INFINITE };

    struct Configuration: public SELECT(Periodic_Thread::Configuration)
    {
    private:
        typedef SELECT(Periodic_Thread::Configuration) Base;
    public:
        Configuration(const Microsecond & p, int n = INFINITE, const State & s = READY, const Criterion & c = NORMAL, Task * t = 0, unsigned int ss = STACK_SIZE)
        : Base(t ? reinterpret_cast<_SYS::Task *>(reinterpret_cast<_SYS::Proxy<_SYS::Task> *>(t->__stub())->id().unit()) : 0,
                p,
                n,
                (_SYS::Thread::State &) s,
                ss,
                reinterpret_cast<Sys_Criterion &>(const_cast<_SYS::Type_Id &>(const_cast<Criterion &>(c).__stub()->id().unit()))
              ) /* For some reason if criterion is not the last,
                 * everything that goes after criterion won't
                 * (des)serialize correctly. */
        {
            db<void>(TRC) << "Periodic_Thread::Configuration constructor (application side): " << endl;
            db<void>(TRC) << "period = " << p
                            << ", times = " << n
                            << ", state = " << s
                            << ", criterion = " << c
                            << ", task (app obj) = " << t
                            << ", task (sys obj) = " << reinterpret_cast<void *>(t->__stub()->id().unit())
                            << ", stack_size = " << ss << endl;
        }

        friend Debug & operator<<(Debug & db, const Configuration & conf)
        {
            _SYS::Message msg(_SYS::Id(_SYS::PERIODIC_THREAD_CONFIGURATION_ID, 0), _SYS::Message::PRINT, const_cast<Configuration &>(conf).__stub()->id().unit());
            msg.act();
            return db;
        }
    };


public:
    template<typename ... Tn>
    Periodic_Thread(const Configuration & conf, int (* entry)(Tn ...), Tn ... an)
    : Base(const_cast<Configuration &>(conf).__stub()->id().unit(),
            entry,
            an ...)
    {
    }

};

// BIND(Network);
class Network: public SELECT(Network)
{
private:
    typedef SELECT(Network) Base;
public:
    static void init() { Base::init_network(); }
};

// BIND(NIC);
class NIC: public SELECT(NIC)
{
private:
    typedef SELECT(NIC) Base;
    typedef _SYS::NIC Sys;

public:
    typedef Sys::Protocol Protocol;

public:
    class Statistics: SELECT(NIC::Statistics)
    {
    public:
        unsigned int get_rx_packets()
        {
            return __stub()->__invoke(_SYS::Message::NIC_STATISTICS_RX_PACKETS);
        }

        unsigned int get_rx_bytes()
        {
            return __stub()->__invoke(_SYS::Message::NIC_STATISTICS_RX_BYTES);
        }

        unsigned int get_tx_packets()
        {
            return __stub()->__invoke(_SYS::Message::NIC_STATISTICS_TX_PACKETS);
        }

        unsigned int get_tx_bytes()
        {
            return __stub()->__invoke(_SYS::Message::NIC_STATISTICS_TX_BYTES);
        }
    };

    class Address: SELECT(NIC::Address)
    {
    public:
        friend OStream & operator<<(OStream & db, const Address & addr)
        {
            _SYS::Message msg(_SYS::Id(_SYS::NIC_ID, 0), _SYS::Message::NIC_ADDRESS_PRINT, const_cast<Address &>(addr).__stub()->id().unit());
            msg.act();
            return db;
        }

        Address operator=(const Address & a)
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return a;
        }

        Address operator=(const Address & a) volatile
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return a;
        }

        operator bool() const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return false;
        }

        operator bool() const volatile
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return false;
        }

        bool operator==(const Address & a) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return false;
        }

        bool operator!=(const Address & a) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return false;
        }

        Address operator&(const Address & a) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return a;
        }

        Address operator|(const Address & a) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return a;
        }

        Address operator~() const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            Address ret;
            return ret;
        }

        unsigned int operator%(unsigned int i) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return 0;
        }

        unsigned char & operator[](const size_t i)
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return _address_field;
        }

        const unsigned char & operator[](const size_t i) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return _address_field;
        }

    private:
        unsigned char _address_field; // Temporary. Remove that later.
    };

public:
    NIC::Statistics & statistics() { return *(reinterpret_cast<NIC::Statistics*>(Base::statistics())); }

    NIC::Address & address() { return *(reinterpret_cast<NIC::Address *>(Base::nic_address())); }

    unsigned int mtu() { return Base::nic_mtu(); }

#if 0
    int receive(Address * src, Protocol * prot, void * data, unsigned int size)
    {
        return Base::nic_receive(reinterpret_cast<Sys::Address>(src->__stub()->id().unit()),
                                    prot,
                                    data,
                                    size);
    }
#endif

    int receive(void * data, unsigned int size) { return Base::nic_receive(0, 0, data, size); }

};

// BIND(IP);
class IP: public SELECT(IP)
{
private:
    typedef SELECT(IP) Base;
    typedef _SYS::IP Sys;

public:
    class Address: SELECT(IP::Address)
    {
    public:
        friend OStream & operator<<(OStream & db, const Address & addr)
        {
            _SYS::Message msg(_SYS::Id(_SYS::IP_ADDRESS_ID, 0), _SYS::Message::IP_ADDRESS_PRINT, const_cast<Address &>(addr).__stub()->id().unit());
            msg.act();
            return db;
        }

        Address operator=(const Address & a)
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return a;
        }

        Address operator=(const Address & a) volatile
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return a;
        }

        operator bool() const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return false;
        }

        operator bool() const volatile
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return false;
        }

        bool operator==(const Address & a) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return false;
        }

        bool operator!=(const Address & a) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return false;
        }

        Address operator&(const Address & a) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return a;
        }

        Address operator|(const Address & a) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return a;
        }

        Address operator~() const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            Address ret;
            return ret;
        }

        unsigned int operator%(unsigned int i) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return 0;
        }

        unsigned char & operator[](const size_t i)
        {
            // _SYS::Message msg(_SYS::Id(_SYS::IP_ID, 0), _SYS::Message::IP_ADDRESS_ARRAY_SUBSCRIPT, i);
            // msg.act();
            // return ((unsigned char &) msg.result());
            _address_field = __stub()->__invoke(_SYS::Message::IP_ADDRESS_ARRAY_SUBSCRIPT, i);
            return _address_field;
        }

        const unsigned char & operator[](const size_t i) const
        {
            db<void>(ERR) << "TODO: method not implemented" << endl;
            return _address_field;
        }

    private:
        unsigned char _address_field;
    };

public:
    NIC * nic() { return reinterpret_cast<NIC*>(Base::nic()); }

    static IP * get_by_nic(unsigned int unit) { return reinterpret_cast<IP*>(Base::get_by_nic(unit)); }

public:
    IP::Address & address()
    {
        db<void>(TRC) << "IP Handle, IP::address" << endl;
        return *(reinterpret_cast<IP::Address*>(Base::address()));
    }
};

// BIND(TCP);
class TCP: public SELECT(TCP)
{
private:
    typedef SELECT(TCP) Base;
    typedef _SYS::TCP Sys;

public:
    typedef Sys::Port Port;

public:
    static const bool connectionless = Sys::connectionless;
};

// BIND(TCP_Link);
class TCP_Link: public SELECT(TCP_Link)
{
private:
    typedef SELECT(TCP_Link) Base;

public:
    TCP_Link(TCP::Port local): Base(local) {};

    int read(void * data, unsigned int size)
    {
        return Base::tcp_link_read(data, size);
    }
};


class Ether_Channel_Link: public SELECT(Ether_Channel_Link)
{
private:
    typedef SELECT(Ether_Channel_Link) Base;

public:
    Ether_Channel_Link(): Base() {};

    int read(void * data, unsigned int size)
    {
        return Base::ether_channel_link_read(data, size);
    }
};


template<typename Channel, bool connectionless = Channel::connectionless>
class Link: public _SYS::IF<(_SYS::Traits<_SYS::Link<Channel, connectionless> >::ASPECTS::Length || (_SYS::Traits<_SYS::Build>::MODE == _SYS::Traits<_SYS::Build>::KERNEL)), _SYS::Handle<_SYS::Link<Channel, connectionless> >, _SYS::Link<Channel, connectionless> >::Result
{
private:
    typedef typename _SYS::IF<(_SYS::Traits<_SYS::Link<Channel, connectionless> >::ASPECTS::Length || (_SYS::Traits<_SYS::Build>::MODE == _SYS::Traits<_SYS::Build>::KERNEL)), _SYS::Handle<_SYS::Link<Channel, connectionless> >, _SYS::Link<Channel, connectionless> >::Result Base;

public:
    template<typename ... Tn>
    Link(const Tn & ... an): Base(an ...) {};
};

template<typename Channel, bool connectionless = Channel::connectionless>
class Port: public _SYS::IF<(_SYS::Traits<_SYS::Port<Channel, connectionless> >::ASPECTS::Length || (_SYS::Traits<_SYS::Build>::MODE == _SYS::Traits<_SYS::Build>::KERNEL)), _SYS::Handle<_SYS::Port<Channel, connectionless> >, _SYS::Port<Channel, connectionless> >::Result
{
private:
    typedef typename _SYS::IF<(_SYS::Traits<_SYS::Port<Channel, connectionless> >::ASPECTS::Length || (_SYS::Traits<_SYS::Build>::MODE == _SYS::Traits<_SYS::Build>::KERNEL)), _SYS::Handle<_SYS::Port<Channel, connectionless> >, _SYS::Port<Channel, connectionless> >::Result Base;

public:
    template<typename ... Tn>
    Port(const Tn & ... an): Base(an ...) {};
};

__END_API

#endif
