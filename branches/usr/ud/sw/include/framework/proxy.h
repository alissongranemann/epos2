// EPOS Component Framework - Component Proxy

// Proxies and Agents handle RMI within EPOS component framework

#ifndef __proxy_h
#define __proxy_h

#include "message.h"

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
class Proxy: public IF<Traits<Component>::ASPECTS::template Count<Hardware>::Result, Message_Hardware, Message_Software>::Result
{
    template<typename> friend class Proxy;
    template<typename> friend class Proxied;

private:
    typedef typename IF<Traits<Component>::ASPECTS::template Count<Hardware>::Result, Message_Hardware, Message_Software>::Result Base;

    typedef typename Base::Methods Methods;
    typedef typename Base::Method Method;
    typedef typename Base::Result Result;

private:
    enum Private_Proxied{ PROXIED };

private:
    Proxy(const Id & id): Base(id) {} // for Proxied::operator new()
    Proxy(const Private_Proxied & p) { db<Framework>(TRC) << "Proxy(PROXIED) => [id=" << Proxy<Component>::id() << "]" << endl; } // for Proxied

public:
    template<typename ... Tn>
    Proxy(const Tn & ... an): Base(Id(Type<Component>::ID, 0)) { invoke(Methods::CREATE + sizeof ... (Tn), an ...); }
    ~Proxy() { invoke(Methods::DESTROY); }

    static Proxy<Component> * self() { return new (reinterpret_cast<void *>(static_invoke(Methods::SELF))) Proxied<Component>; }

    // Process management
    void suspend() { invoke(Methods::THREAD_SUSPEND); }
    void resume() { invoke(Methods::THREAD_RESUME); }
    int join() { return invoke(Methods::THREAD_JOIN); }
    int pass() { return invoke(Methods::THREAD_PASS); }
    static int yield() { return static_invoke(Methods::THREAD_YIELD); }
    static void exit(int r) { static_invoke(Methods::THREAD_EXIT, r); }
    static volatile bool wait_next() { return static_invoke(Methods::THREAD_WAIT_NEXT); }

    Proxy<Address_Space> * address_space() { return new (reinterpret_cast<Adapter<Address_Space> *>(invoke(Methods::TASK_ADDRESS_SPACE))) Proxied<Address_Space>; }
    Proxy<Segment> * code_segment() { return new (reinterpret_cast<Adapter<Segment> *>(invoke(Methods::TASK_CODE_SEGMENT))) Proxied<Segment>; }
    Proxy<Segment> * data_segment() { return new (reinterpret_cast<Adapter<Segment> *>(invoke(Methods::TASK_DATA_SEGMENT))) Proxied<Segment>; }
    CPU::Log_Addr code() { return invoke(Methods::TASK_CODE); }
    CPU::Log_Addr data() { return invoke(Methods::TASK_DATA); }

    // Memory management
    CPU::Phy_Addr pd() { return invoke(Methods::ADDRESS_SPACE_PD); }
    CPU::Log_Addr attach(const Proxy<Segment> & seg) { return invoke(Methods::ADDRESS_SPACE_ATTACH1, seg.id().unit()); }
    CPU::Log_Addr attach(const Proxy<Segment> & seg, CPU::Log_Addr addr) { return invoke(Methods::ADDRESS_SPACE_ATTACH2, seg.id().unit(), addr); }
    void detach(const Proxy<Segment> & seg) { invoke(Methods::ADDRESS_SPACE_DETACH, seg.id().unit());}

    unsigned int size() { return invoke(Methods::SEGMENT_SIZE); }
    CPU::Phy_Addr phy_address() { return invoke(Methods::SEGMENT_PHY_ADDRESS); }
    int resize(int amount) { return invoke(Methods::SEGMENT_RESIZE, amount); }

    // Synchronization
    void lock() { invoke(Methods::SYNCHRONIZER_LOCK); }
    void unlock() { invoke(Methods::SYNCHRONIZER_UNLOCK); }

    void p() { invoke(Methods::SYNCHRONIZER_P); }
    void v() { invoke(Methods::SYNCHRONIZER_V); }

    void wait() { invoke(Methods::SYNCHRONIZER_WAIT); }
    void signal() { invoke(Methods::SYNCHRONIZER_SIGNAL); }
    void broadcast() { invoke(Methods::SYNCHRONIZER_BROADCAST); }

    // Timing
    template<typename T>
    static void delay(T t) { static_invoke(Methods::ALARM_DELAY, t); }

    // Communication
    template<typename T1, typename T2, typename T3>
    int send(T1 a1, T2 a2, T3 a3) { return invoke(Methods::SELF, a1, a2, a3); }
    template<typename T1, typename T2, typename T3>
    int receive(T1 a1, T2 a2, T3 a3) { return invoke(Methods::SELF, a1, a2, a3); }

    // Adder
    int add(int a, int b) { return invoke(Methods::ADDER_ADD, a, b); }
    int save_st() { return invoke(Methods::ADDER_SAVE_ST); }
    int get_st_len() { return invoke(Methods::ADDER_GET_ST_LEN); }

    template<typename ... Tn>
    static int static_invoke(const Method & m, const Tn & ... an) {
        Base msg(Id(Type<Component>::ID, 0)); // avoid calling ~Proxy()
        Result res = msg.act(m, an ...);
        return (m == Methods::SELF) ? msg.id().unit() : res;
    }

private:
    template<typename ... Tn>
    int invoke(const Method & m, const Tn & ... an) { return act(m, an ...); }
};

__END_SYS

#endif
