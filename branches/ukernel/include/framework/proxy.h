// EPOS Component Framework - Component Proxy

// Proxies and Agents handle RMI within EPOS component framework

#ifndef __proxy_h
#define __proxy_h

#include "message.h"
#include "ipc.h"

__BEGIN_SYS

template<typename Component>
class Proxy;

// Proxied is used to create a Proxy for components created either by SETUP before the framework came into place or internally by the framework itself
template<typename Component>
class Proxied: public Proxy<Component>
{
public:
    Proxied(): Proxy<Component>(Proxy<Component>::PROXIED) {
        db<Framework>(TRC) << "Proxied(this=" << this << ")" << endl;
    }

    void * operator new(size_t s, void * adapter) {
        db<Framework>(TRC) << "Proxied::new(adapter=" << adapter << ")" << endl;
        Framework::Element * el= Framework::_cache.search_key(reinterpret_cast<unsigned int>(adapter));
        void * proxy;
        if(el) {
            proxy = el->object();
            db<Framework>(INF) << "Proxied::new(adapter=" << adapter << ") => " << proxy << " (CACHED)" << endl;
        } else {
            proxy = new Proxy<Component>(Id(Type<Component>::ID, reinterpret_cast<Id::Unit_Id>(adapter)));
            el = new Framework::Element(proxy, reinterpret_cast<unsigned int>(adapter));  // the proxied cache is insert-only; object are intentionally never deleted, since they have been created by SETUP!
            Framework::_cache.insert(el);
        }
        return proxy;
    }
};

template<typename Component>
class Proxy: public Message
{
    template<typename> friend class Proxy;
    template<typename> friend class Proxied;

private:
    enum Private_Proxied{ PROXIED };

private:
    Proxy(const Id & id): Message(id) {} // for Proxied::operator new()
    Proxy(const Private_Proxied & p) { db<Framework>(TRC) << "Proxy(PROXIED) => [id=" << Proxy<Component>::id() << "]" << endl; } // for Proxied

public:
    template<typename ... Tn>
    Proxy(const Tn & ... an): Message(Id(Type<Component>::ID, 0)) { invoke(CREATE + sizeof ... (Tn), an ...); }
    ~Proxy() { invoke(DESTROY); }

    static Proxy<Component> * self() { return new (reinterpret_cast<void *>(static_invoke(SELF))) Proxied<Component>; }

    // Process management
    int state() { return invoke(THREAD_STATE); }
    int priority() { return invoke(THREAD_PRIORITY); }
    void priority(int p) { invoke(THREAD_PRIORITY1, p); }
    int join() { return invoke(THREAD_JOIN); }
    int pass() { return invoke(THREAD_PASS); }
    void suspend() { invoke(THREAD_SUSPEND); }
    void resume() { invoke(THREAD_RESUME); }
    static int yield() { return static_invoke(THREAD_YIELD); }
    static void exit(int r) { static_invoke(THREAD_EXIT, r); }
    static volatile bool wait_next() { return static_invoke(PERIODIC_THREAD_WAIT_NEXT); }

    Proxy<Address_Space> * address_space() { return new (reinterpret_cast<Adapter<Address_Space> *>(invoke(TASK_ADDRESS_SPACE))) Proxied<Address_Space>; }
    Proxy<Segment> * code_segment() { return new (reinterpret_cast<Adapter<Segment> *>(invoke(TASK_CODE_SEGMENT))) Proxied<Segment>; }
    Proxy<Segment> * data_segment() { return new (reinterpret_cast<Adapter<Segment> *>(invoke(TASK_DATA_SEGMENT))) Proxied<Segment>; }
    CPU::Log_Addr code() { return invoke(TASK_CODE); }
    CPU::Log_Addr data() { return invoke(TASK_DATA); }
    Proxy<Thread> * main() { return new (reinterpret_cast<Adapter<Thread> *>(invoke(TASK_MAIN))) Proxied<Thread>; }
    void main(Proxy<Thread> * thread) { invoke(TASK_SET_MAIN, thread->id().unit()); }

    // Memory management
    CPU::Phy_Addr pd() { return invoke(ADDRESS_SPACE_PD); }
    CPU::Log_Addr attach(const Proxy<Segment> & seg) { return invoke(ADDRESS_SPACE_ATTACH1, seg.id().unit()); }
    CPU::Log_Addr attach(const Proxy<Segment> & seg, CPU::Log_Addr addr) { return invoke(ADDRESS_SPACE_ATTACH2, seg.id().unit(), addr); }
    void detach(const Proxy<Segment> & seg) { invoke(ADDRESS_SPACE_DETACH, seg.id().unit());}

    unsigned int size() { return invoke(SEGMENT_SIZE); }
    CPU::Phy_Addr phy_address() { return invoke(SEGMENT_PHY_ADDRESS); }
    int resize(int amount) { return invoke(SEGMENT_RESIZE, amount); }

    static unsigned long physical_address(unsigned long log_addr, unsigned long * out_page_frame_present) { return static_invoke(MMU_AUX_PHYSICAL_ADDRESS, log_addr, out_page_frame_present); }
    static void dump_memory_mapping() { static_invoke(MMU_AUX_DUMP_MEMORY_MAPPING); }
    static void check_memory_mapping() { static_invoke(MMU_AUX_CHECK_MEMORY_MAPPING); }
    static void set_as_read_only(unsigned long log_addr, unsigned long size, bool user = 1) { static_invoke(MMU_AUX_SET_AS_READ_ONLY, log_addr, size, user); }

    // Synchronization
    void lock() { invoke(SYNCHRONIZER_LOCK); }
    void unlock() { invoke(SYNCHRONIZER_UNLOCK); }

    void p() { invoke(SYNCHRONIZER_P); }
    void v() { invoke(SYNCHRONIZER_V); }

    void wait() { invoke(SYNCHRONIZER_WAIT); }
    void signal() { invoke(SYNCHRONIZER_SIGNAL); }
    void broadcast() { invoke(SYNCHRONIZER_BROADCAST); }

    // Timing
    template<typename T>
    static void delay(T t) { static_invoke(ALARM_DELAY, t); }

    static TSC::Time_Stamp time_stamp() { return static_invoke(TSC_TIME_STAMP); }

    static Chronometer_Aux::Nanosecond elapsed_nano(TSC::Time_Stamp start, TSC::Time_Stamp stop)
    {
        return static_invoke(CHRONO_ELAPSED_NANO, start, stop);
    }

    static Chronometer_Aux::Microsecond elapsed_micro(TSC::Time_Stamp start, TSC::Time_Stamp stop)
    {
        return static_invoke(CHRONO_ELAPSED_MICRO, start, stop);
    }

    static Chronometer_Aux::Second elapsed_sec(TSC::Time_Stamp start, TSC::Time_Stamp stop)
    {
        return static_invoke(CHRONO_ELAPSED_SEC, start, stop);
    }

    static Chronometer_Aux::Nanosecond nano(TSC::Time_Stamp ticks)
    {
        return static_invoke(CHRONO_NANO, ticks);
    }

    static Chronometer_Aux::Microsecond micro(TSC::Time_Stamp ticks)
    {
        return static_invoke(CHRONO_MICRO, ticks);
    }

    static Chronometer_Aux::Second sec(TSC::Time_Stamp ticks)
    {
        return static_invoke(CHRONO_SEC, ticks);
    }

    // Communication
    template<typename ... Tn>
    int send(Tn ... an) { return invoke(IPC_SEND, an ...); }
    template<typename ... Tn>
    int receive(Tn ... an) { return invoke(IPC_RECEIVE, an ...); }

    template<typename ... Tn>
    int read(Tn ... an) { return receive(an ...); }
    template<typename ... Tn>
    int write(Tn ... an) { return send(an ...); }

    int tcp_link_read(void * data, unsigned int size) { return invoke(TCP_LINK_READ, data, size); }

    int ether_channel_link_read(void * data, unsigned int size) { return invoke(ETHER_CHANNEL_LINK_READ, data, size); }

    // Network
    static void init_network() { static_invoke(NETWORK_INIT); }

    // NIC
    Proxy<NIC::Statistics> * statistics() { return new (reinterpret_cast<Adapter<NIC::Statistics> *>(invoke(NIC_STATISTICS))) Proxied<NIC::Statistics>; }
    Proxy<NIC::Address> * nic_address() { return new (reinterpret_cast<Adapter<NIC::Address> *>(invoke(NIC_ADDRESS))) Proxied<NIC::Address>; }
    unsigned int nic_mtu() { return invoke(NIC_MTU); }
    int nic_receive(NIC::Address * src, NIC::Protocol * prot, void * data, unsigned int size) { return invoke(NIC_RECEIVE, src, prot, data, size); }

    // IP
    Proxy<NIC> * nic() { return new (reinterpret_cast<Adapter<NIC> *>(invoke(IP_NIC))) Proxied<NIC>; }
    Proxy<IP::Address> * address() { return new (reinterpret_cast<Adapter<IP::Address> *>(invoke(IP_ADDRESS))) Proxied<IP::Address>; }
    static Proxy<IP> * get_by_nic(unsigned int unit) { return new (reinterpret_cast<Adapter<IP> *>(static_invoke(IP_GET_BY_NIC, unit))) Proxied<IP>; }

    // Machine
    static void smp_barrier() { static_invoke(MACHINE_SMP_BARRIER); }
    static unsigned int cpu_id() { return static_invoke(MACHINE_CPU_ID); }

    // This_Thread
    static unsigned int this_thread_id() { return static_invoke(THIS_THREAD_ID_ID); }

    // CPU
    static void int_enable() { static_invoke(CPU_INT_ENABLE); }
    static void int_disable() { static_invoke(CPU_INT_DISABLE); }

public:
    template<typename ... Tn>
    Result __invoke(const Method & m, const Tn & ... an) {
        return invoke(m, an ...);
    }

private:
    template<typename ... Tn>
    Result invoke(const Method & m, const Tn & ... an) {
        method(m);
        out(an ...);
        act();
        return result();
    }

    template<typename ... Tn>
    static Result static_invoke(const Method & m, const Tn & ... an) {
        Message msg(Id(Type<Component>::ID, 0)); // avoid calling ~Proxy()
        msg.method(m);
        msg.out(an ...);
        msg.act();
        return (m == SELF) ? msg.id().unit() : msg.result();
    }
};

__END_SYS

#endif
