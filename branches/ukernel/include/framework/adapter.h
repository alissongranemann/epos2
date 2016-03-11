// EPOS Component Framework - Scenario Adapter

// Scenario adapters are the heart of EPOS component framework.
// They collect component-specific Aspect programs to build a scenario for it to run.
// Scenario features are enforced by wrapping all and any method invocation (event creation and destruction)
// within the enter() and leave() methods.

#ifndef __adapter_h
#define __adapter_h

#include "scenario.h"

__BEGIN_SYS

template<typename Component>
class Adapter: public Component, public Scenario<Component>
{
    using Scenario<Component>::enter;
    using Scenario<Component>::leave;
    using Scenario<Component>::static_enter;
    using Scenario<Component>::static_leave;

public:
    typedef Component _Component; // used by Message

public:
    template<typename ... Tn>
    Adapter(const Tn & ... an): Component(an ...) { static_leave(); }
    ~Adapter() { static_enter(); }

    void * operator new(size_t bytes) {
        static_enter();
        return Scenario<Component>::operator new(bytes);
    }
    void operator delete(void * adapter) {
        Scenario<Component>::operator delete(adapter);
        static_leave();
    }

    static const Adapter<Component> * self() { static_enter(); const Adapter<Component> * res = reinterpret_cast<const Adapter<Component> *>(Component::self()); return res; }

    // Process management
    void suspend() { enter(); Component::suspend(); leave(); }
    void resume() { enter(); Component::resume(); leave(); }
    int join() { enter(); int res = Component::join(); leave(); return res; }
    void pass() { enter(); Component::pass(); leave(); }
    static void yield() { static_enter(); Component::yield(); static_leave(); }
    static void exit(int status) { static_enter(); Component::exit(status); static_leave(); }

    Address_Space * address_space() { enter(); Address_Space * res = Component::address_space(); leave(); return res; }
    Segment * code_segment() { enter(); Segment * res = Component::code_segment(); leave(); return res; }
    Segment * data_segment() { enter(); Segment * res = Component::data_segment(); leave(); return res; }
    CPU::Log_Addr code() { enter(); CPU::Log_Addr res = Component::code(); leave(); return res; }
    CPU::Log_Addr data() { enter(); CPU::Log_Addr res = Component::data(); leave(); return res; }
    Thread * main() { enter(); Thread * res = Component::main(); leave(); return res; }
    void main(Thread * thread) { enter(); Component::main(thread); leave(); }
    static void setup() { static_enter(); Component::setup(); static_leave(); }

    // Memory management
    CPU::Phy_Addr pd() { enter(); CPU::Phy_Addr res = Component::pd(); leave(); return res; }
    CPU::Log_Addr attach(Segment * seg) { enter(); CPU::Log_Addr res = Component::attach(seg); leave(); return res; }
    CPU::Log_Addr attach(Segment * seg, const CPU::Log_Addr & addr) { enter(); CPU::Log_Addr res = Component::attach(seg, addr); leave(); return res; }
    void detach(Segment * seg) { enter(); Component::detach(seg); leave(); }

    unsigned int size() { enter(); unsigned int res = Component::size(); leave(); return res; }
    CPU::Phy_Addr phy_address() { enter(); CPU::Phy_Addr res = Component::phy_address(); leave(); return res; }
    int resize(int amount) { enter(); int res = Component::resize(amount); leave(); return res; }

    // Synchronization
    void lock() { enter(); Component::lock(); leave(); }
    void unlock() { enter(); Component::unlock(); leave(); }
    void p() { enter(); Component::p(); leave(); }
    void v() { enter(); Component::v(); leave(); }
    void wait() { enter(); Component::wait(); leave(); }
    void signal() { enter(); Component::signal(); leave(); }
    void broadcast() { enter(); Component::broadcast(); leave(); }

    // Timing
    static void delay(const RTC::Microsecond & time) { static_enter(); Component::delay(time); static_leave(); }
    void reset() { enter(); Component::reset(); leave(); }
    void start() { enter(); Component::start(); leave(); }
    void lap() { enter(); Component::lap(); leave(); }
    void stop() { enter(); Component::stop(); leave(); }
    int frequency() { enter(); int res = Component::frequency(); leave(); return res; }
    int ticks() { enter(); int res = Component::ticks(); leave(); return res; }
    int read() { enter(); int res = Component::read(); leave(); return res; }

    // Communication
    template<typename ... Tn>
    int send(Tn ... an) {
        enter();
        int res = Component::send(an ...);
        leave();
        return res;
    }
    template<typename ... Tn>
    int receive(Tn ... an) {
        enter();
        int res = Component::receive(an ...);
        leave();
        return res;
    }

    template<typename ... Tn>
    int read(Tn ... an) { return receive(an ...);}
    template<typename ... Tn>
    int write(Tn ... an) { return send(an ...);}

    // Network
    static void init_network() { static_enter(); Component::init(); static_leave(); }

    // NIC
    NIC::Statistics * statistics() { enter(); NIC::Statistics * res = Component::statistics_prt(); leave(); return res; }
    NIC::Address * nic_address() { enter(); NIC::Address * res = &(const_cast<NIC::Address &>(Component::address())); leave(); return res; }

    // NIC::Statistics
    unsigned int rx_packets() { enter(); unsigned int res = Component::rx_packets; leave(); return res; }
    unsigned int rx_bytes() { enter(); unsigned int res = Component::rx_bytes; leave(); return res; }
    unsigned int tx_packets() { enter(); unsigned int res = Component::tx_packets; leave(); return res; }
    unsigned int tx_bytes() { enter(); unsigned int res = Component::tx_bytes; leave(); return res; }

    // IP
    NIC * nic() { enter(); NIC * res = Component::nic(); leave(); return res; }
    IP::Address * address() { enter(); IP::Address * res = Component::address_prt(); leave(); return res; }
    static IP * get_by_nic(unsigned int unit) { static_enter(); IP * res = Component::get_by_nic(unit); static_leave(); return res; }

    // Machine
    static void smp_barrier() { static_enter(); Component::smp_barrier(); static_leave(); }
    static unsigned int cpu_id() { static_enter(); int res = Component::cpu_id(); static_leave(); return res; }

    // This_Thread
    static unsigned int this_thread_id() { static_enter(); unsigned res = Component::id(); static_leave(); return res; }

    // CPU
    static void int_enable() { static_enter(); Component::int_enable(); static_leave(); }
    static void int_disable() { static_enter(); Component::int_disable(); static_leave(); }

};

__END_SYS

#endif
