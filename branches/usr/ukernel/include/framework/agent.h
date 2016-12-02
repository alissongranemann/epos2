// EPOS Component Framework - Component Agent

#ifndef __agent_h
#define __agent_h

#include <thread.h>
#include <task.h>
#include <active.h>
#include <address_space.h>
#include <segment.h>
#include <mutex.h>
#include <semaphore.h>
#include <condition.h>
#include <clock.h>
#include <alarm.h>
#include <chronometer.h>
#include <communicator.h>
#include <color.h>

#include <architecture/ia32/mmu_aux.h>

#include "message.h"
#include "ipc.h"

__BEGIN_SYS

class Agent: public Message
{

    friend class Init_System;

private:
    typedef void (Agent:: * Member)();

public:
    void exec()
    {
        // db<void>(WRN) << "exec, Component = " << id().type() << ", method = " << method() << endl;

        if (id().type() == UNKNOWN_TYPE_ID) {
            db<Framework>(ERR) << "Error in exec. UNKNOWN_TYPE_ID" << endl;
        }

        if (id().type() != UTILITY_ID) {
            db<Framework>(TRC) << ":=>" << *reinterpret_cast<Message *>(this) << endl;
        }

        if (id().type() < LAST_TYPE_ID) { // in-kernel services
            if (_handlers[id().type()]) {
                (this->*_handlers[id().type()])();
            }
            else {
                db<Framework>(ERR) << "Error: handler for type " << id().type() << " not registered." << endl;
            }
        }
        else { // out-of-kernel (i.e. Dom0 or server) services
                db<void>(WRN) << "out-of-kernel (i.e. Dom0 or server) services. Type: " << reinterpret_cast<void *>(id().type()) << endl;

                Message msg(*this); // copy message from user space to kernel
                msg.id(Id(IPC_COMMUNICATOR_ID, id().unit()));
                if (IPC::send(id().unit(), &msg)) { // 0 => no one listening
                    Port<IPC> * comm = reinterpret_cast<Port<IPC> *>(IPC::observer(id().type())); // recall the Port<IPC> that got us here
                    db<void>(WRN) << "will wait for reply" << endl;
                    comm->receive(this); // copy from kernel to user
                } else
                    result(UNDEFINED);
        }

        if (id().type() != UTILITY_ID)
            db<Framework>(TRC) << "<=:" << *reinterpret_cast<Message *>(this) << endl;

        if (result() == UNDEFINED) {
            db<Framework>(ERR) << "exec. Undefined method for Component = " << reinterpret_cast<void *>(id().type()) << ", method = " << method() << endl;
        }
    }

private:
    void handle_thread();
    void handle_task();
    void handle_active();
    void handle_address_space();
    void handle_segment();
    void handle_mutex();
    void handle_semaphore();
    void handle_condition();
    void handle_clock();
    void handle_alarm();
    void handle_chronometer();
    void handle_ipc();
    void handle_utility();
    void handle_machine();
    void handle_this_thread();
    void handle_network();
    void handle_nic();
    void handle_nic_statistics();
    void handle_ip();
    void handle_ip_address();
    void handle_cpu();
    void handle_pedf();
    void handle_periodic_thread();
    void handle_thread_configuration();
    void handle_periodic_thread_configuration();
    void handle_tcp_link();
    void handle_ether_channel_link();
    void handle_mmu_aux();
    void handle_tsc();
    void handle_chronometer_aux();
    void handle_fpga();
    void handle_uart();

public:
    static void init();

private:
    static Member _handlers[LAST_TYPE_ID];
};


void Agent::handle_thread()
{
    Adapter<Thread> * thread = reinterpret_cast<Adapter<Thread> *>(id().unit());
    Result res = 0;

    db<Framework>(TRC) << "Agent::handle_thread, method = " << method() << endl;

    switch(method()) {
    case CREATE1: {
        db<Framework>(TRC) << "Thread Agent, CREATE1" << endl;
        int (*entry)();
        in(entry);
        id(Id(THREAD_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Thread>(Thread::Configuration(Thread::READY, Thread::NORMAL, 0, 0), entry))));
    } break;
    case CREATE2: {
        db<Framework>(TRC) << "Thread Agent, CREATE2" << endl;
        Thread::Configuration * conf;
        int (*entry)();
        in(conf, entry);
        db<Framework>(TRC) << "entry: " << reinterpret_cast<void *>(entry)
                            << ", conf: " << *conf
                            << ", conf obj: " << reinterpret_cast<void *>(conf) << endl;
        id(Id(THREAD_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Thread>(*conf, entry))));
    } break;
    case DESTROY: {
        delete thread;
    } break;
    case SELF: {
        id(Id(THREAD_ID, reinterpret_cast<Id::Unit_Id>(Adapter<Thread>::self())));
    } break;
    case THREAD_PRIORITY: {
        res = thread->priority();
    } break;
    case THREAD_PRIORITY1: {
        int p;
        in(p);
        thread->priority(p);
    } break;
    case THREAD_JOIN: {
        res = thread->join();
    } break;
    case THREAD_PASS: {
        thread->pass();
    } break;
    case THREAD_SUSPEND: {
        thread->suspend();
    } break;
    case THREAD_RESUME: {
        thread->resume();
    } break;
    case THREAD_YIELD: {
        Thread::yield();
    } break;
    case THREAD_EXIT: {
        int r;
        in(r);
        Thread::exit(r);
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Thread agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_periodic_thread()
{
    db<Framework>(TRC) << "Agent::handle_periodic_thread, method = " << method() << endl;

    Adapter<Periodic_Thread> * thread = reinterpret_cast<Adapter<Periodic_Thread> *>(id().unit());
    Result res = 0;

    switch(method()) {
    case CREATE2: {
        db<Framework>(TRC) << "Agent::handle_periodic_thread, CREATE2" << endl;
        Periodic_Thread::Configuration * conf;
        int (* entry)();
        in(conf, entry);

        db<Framework>(TRC) << "conf (obj) = " << conf << endl;
        db<Framework>(TRC) << "conf = " << *conf << endl;

        id(Id(PERIODIC_THREAD_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Periodic_Thread>(*conf, entry))));

        db<Framework>(TRC) << "created = " << reinterpret_cast<void *>(id().unit()) << endl;

    } break;
    case CREATE3: {
        db<Framework>(TRC) << "Agent::handle_periodic_thread, CREATE3" << endl;
        Periodic_Thread::Configuration * conf;
        int (* entry)(unsigned int);
        unsigned int a1;

        in(conf, entry, a1);

        db<Framework>(TRC) << "conf (obj) = " << conf << endl;
        db<Framework>(TRC) << "conf = " << *conf << endl;

        id(Id(PERIODIC_THREAD_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Periodic_Thread>(*conf, entry, a1))));

        db<Framework>(TRC) << "created = " << reinterpret_cast<void *>(id().unit()) << endl;

    } break;
    case DESTROY: {
        delete thread;
    } break;
    case PERIODIC_THREAD_WAIT_NEXT: {
        bool times = Adapter<Periodic_Thread>::wait_next();
        res = times;
    } break;
    case THREAD_JOIN: {
        res = thread->join();
    } break;
    case THREAD_PASS: {
        thread->pass();
    } break;
    case THREAD_SUSPEND: {
        thread->suspend();
    } break;
    case THREAD_RESUME: {
        thread->resume();
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Periodic_Thread agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_task()
{
    Adapter<Task> * task = reinterpret_cast<Adapter<Task> *>(id().unit());
    Result res = 0;

    switch(method()) {
    case CREATE3: {
        db<Framework>(TRC) << "Task Agent, CREATE3" << endl;
        Segment * cs, * ds;
        int (*entry)();
        in(cs, ds, entry);
        id(Id(TASK_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Task>(cs, ds, entry))));
    } break;
    case CREATE4: {
        db<Framework>(TRC) << "Task Agent, CREATE4" << endl;
        Thread * thread;
        Segment * cs, * ds;
        int (*entry)();
        in(thread, cs, ds, entry);
        id(Id(TASK_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Task>(thread, cs, ds, entry))));
    } break;
    case DESTROY: {
        delete task;
    } break;
    case SELF: {
        id(Id(TASK_ID, reinterpret_cast<Id::Unit_Id>(Adapter<Task>::self())));
    } break;
    case TASK_ADDRESS_SPACE: {
        res = reinterpret_cast<int>(task->address_space());
    } break;
    case TASK_CODE_SEGMENT: {
        res = reinterpret_cast<int>(task->code_segment());
    } break;
    case TASK_DATA_SEGMENT: {
        res = reinterpret_cast<int>(task->data_segment());
    } break;
    case TASK_CODE: {
        res = task->code();
    } break;
    case TASK_DATA: {
        res = task->data();
    } break;
    case TASK_MAIN: {
        res = reinterpret_cast<int>(task->main());
    } break;
    case TASK_SET_MAIN: {
        Thread * thread;
        in(thread);
        task->main(thread);
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Task agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_active()
{
    db<Framework>(WRN) << "Undefined method for Active agent. Method = " << method() << endl;
    result(UNDEFINED);
}


void Agent::handle_address_space()
{
    Adapter<Address_Space> * as = reinterpret_cast<Adapter<Address_Space> *>(id().unit());
    Result res = 0;

    switch(method()) {
    case CREATE: {
        db<Framework>(TRC) << "Address Space Agent, CREATE" << endl;
        id(Id(ADDRESS_SPACE_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Address_Space>())));
    } break;
    case DESTROY: {
        delete as;
    } break;
    case ADDRESS_SPACE_PD: {
        res = as->pd();
    } break;
    case ADDRESS_SPACE_ATTACH1: {
        Segment * seg;
        in(seg);

        db<Framework>(TRC) << Color::YELLOW() << "Address Space Agent, attach 1, seg = " << reinterpret_cast<void *>(seg) << Color::END_COLOR() << endl;

        res = as->attach(seg);
    } break;
    case ADDRESS_SPACE_ATTACH2: {
        Segment * seg;
        CPU::Log_Addr addr;
        in(seg, addr);

        db<Framework>(TRC) << Color::YELLOW() << "Address Space Agent, attach 2, seg = " << reinterpret_cast<void *>(seg) << ", addr = " << addr << Color::END_COLOR() << endl;

        res = as->attach(seg, addr);
    } break;
    case ADDRESS_SPACE_DETACH: {
        Segment * seg;
        in(seg);

        db<Framework>(TRC) << Color::YELLOW() << "Address Space Agent, detach, seg = " << reinterpret_cast<void *>(seg) << Color::END_COLOR() << endl;

        as->detach(seg);
    } break;
    case ADDRESS_PHYSICAL: {
        CPU::Log_Addr addr;
        in(addr);
        res = as->physical(addr);
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Address_Space agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_segment()
{
    Adapter<Segment> * seg = reinterpret_cast<Adapter<Segment> *>(id().unit());
    Result res = 0;

    switch(method()) {
    case CREATE1: {
        db<Framework>(TRC) << "Segment Agent, CREATE1" << endl;
        unsigned int bytes;
        in(bytes);
        id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Segment>(bytes))));
    } break;
    case CREATE2: { // *** indistinguishable ***
        db<Framework>(TRC) << "Segment Agent, CREATE2" << endl;
        unsigned int bytes;
        Segment::Flags flags;
        in(bytes, flags);
        id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Segment>(bytes, flags))));
    } break;
    case CREATE3: { // *** indistinguishable ***
        db<Framework>(TRC) << "Segment Agent, CREATE3" << endl;
        Segment::Phy_Addr phy_addr;
        unsigned int bytes;
        Segment::Flags flags;
        in(phy_addr, bytes, flags);
        id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Segment>(phy_addr, bytes, flags))));
    } break;
    case DESTROY: {
        db<void>(TRC) << "DESTROY" << endl;
        delete seg;
    } break;
    case SEGMENT_SIZE: {
        db<void>(TRC) << "SEGMENT_SIZE" << endl;
        res = seg->size();
    } break;
    case SEGMENT_PHY_ADDRESS: {
        db<void>(TRC) << "SEGMENT_PHY_ADDRESS" << endl;
        res = seg->phy_address();
    } break;
    case SEGMENT_RESIZE: {
        db<void>(TRC) << "SEGMENT_RESIZE" << endl;
        int amount;
        in(amount);
        res = seg->resize(amount);
    } break;
    case CREATE_SEGMENT_IN_PLACE: {
        db<void>(TRC) << "CREATE_SEGMENT_IN_PLACE" << endl;
        void * place;
        unsigned int size;
        unsigned int mmu_flags;
        in(place, size, mmu_flags);
        db<void>(TRC) << "place: " << reinterpret_cast<void *>(place) << ", size: " << size << ", mmu_flags: " << reinterpret_cast<void *>(mmu_flags) << endl;
        new (place) Segment(size, mmu_flags);
        id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(place)));

    } break;
    case CREATE_HEAP_IN_PLACE: {
        db<void>(TRC) << "CREATE_HEAP_IN_PLACE" << endl;

        void * place;
        Segment * heap_segment;
        in(place, heap_segment);

        Address_Space current_address_space = Address_Space(reinterpret_cast<MMU::Page_Directory *>(CPU::cr3()));

        db<void>(TRC) << "place: " << reinterpret_cast<void *>(place) << ", heap_segment: " << reinterpret_cast<void *>(heap_segment) << endl;
        db<void>(TRC) << "current page directory is: " << current_address_space.pd() << endl;
        db<void>(TRC) << "segment size: " << heap_segment->size() << endl;

        CPU::Log_Addr addr = current_address_space.attach(heap_segment);

        db<void>(TRC) << "segment attached to: " << addr << endl;

        new (place) Heap(addr, heap_segment->size());
        id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(place)));
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Segment agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_mutex()
{
    db<Framework>(WRN) << "Undefined method for Mutex agent. Method = " << method() << endl;
    result(UNDEFINED);
}


void Agent::handle_semaphore()
{
    db<Framework>(WRN) << "Undefined method for Semaphore agent. Method = " << method() << endl;
    result(UNDEFINED);
}


void Agent::handle_condition()
{
    db<Framework>(WRN) << "Undefined method for Condition agent. Method = " << method() << endl;
    result(UNDEFINED);
}


void Agent::handle_clock()
{
    db<Framework>(WRN) << "Undefined method for Clock agent. Method = " << method() << endl;
    result(UNDEFINED);
}


void Agent::handle_alarm()
{
    Adapter<Alarm> * alarm = reinterpret_cast<Adapter<Alarm> *>(id().unit());
    Result res = 0;

    switch(method()) {
    case CREATE3: {
        db<Framework>(TRC) << "Alarm Agent, CREATE3" << endl;
        Alarm::Microsecond time;
        Handler * handler;
        int times;
        in(time, handler, times);
        id(Id(ALARM_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Alarm>(time, handler, times))));
    } break;
    case DESTROY: {
        delete alarm;
    } break;
    case ALARM_DELAY: {
        Alarm::Microsecond time;
        in(time);
        Adapter<Alarm>::delay(time);
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Alarm agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_chronometer()
{
    db<Framework>(WRN) << "Undefined method for Chronometer agent. Method = " << method() << endl;
    result(UNDEFINED);
}

void Agent::handle_ipc()
{
    Adapter<Port<IPC>> * comm = reinterpret_cast<Adapter<Port<IPC>> *>(id().unit());
    Result res = 0;

    switch(method()) {
    case CREATE1: {
        db<Framework>(TRC) << "IPC Agent, CREATE1" << endl;
        Port<IPC>::Local_Address local;
        in(local);
        id(Id(IPC_COMMUNICATOR_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Port<IPC>>(local))));
    } break;
    case DESTROY: {
        delete comm;
    } break;
    case IPC_SEND: {
        Message * msg;
        in(msg);
        comm->send(msg);
    } break;
    case IPC_RECEIVE: {
        Message * msg;
        in(msg);
        comm->receive(msg);
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for IPC agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_machine()
{
    Result res = 0;
    switch(method()) {
    case MACHINE_SMP_BARRIER: {
        Adapter<Machine>::smp_barrier();
    } break;
    case MACHINE_CPU_ID: {
        res = Adapter<Machine>::cpu_id();
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Machine agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }
    result(res);
}


void Agent::handle_this_thread()
{
    Result res = 0;

    switch(method()) {
    case THIS_THREAD_ID_ID: {
        res = Adapter<This_Thread>::this_thread_id();
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for This_Thread agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_network()
{
    Result res = 0;

    switch (method()) {
    case NETWORK_INIT: {
        // Adapter<Network>::init_network();
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Network agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_nic()
{
    Adapter<NIC> * nic = reinterpret_cast<Adapter<NIC> *>(id().unit());
    Result res = 0;

    switch (method()) {
    case CREATE: {
        db<Framework>(TRC) << "NIC Agent, CREATE" << endl;

        id(Id(NIC_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<NIC>())));

    } break;
    case DESTROY: {
        // delete nic;

    } break;
    case NIC_STATISTICS: {
        res = reinterpret_cast<Result>(nic->statistics());
    } break;
    case NIC_ADDRESS: {
        res = reinterpret_cast<Result>(nic->nic_address());
    } break;
    case NIC_ADDRESS_PRINT: {
        db<Framework>(TRC) << "NIC::Address Agent, PRINT" << endl;
        NIC::Address * addr;
        in(addr);
        db<void>(WRN) << " " << *addr << endl;
    } break;
    case NIC_MTU: {
        res = nic->nic_mtu();

    } break;
    case NIC_RECEIVE: {
        NIC::Address * src;
        NIC::Protocol * prot;
        void * data;
        unsigned int size;

        in(src, prot, data, size);

        db<Framework>(TRC) << "NIC Agent, NIC_RECEIVE, src = " << src << ", prot = " << prot << ", data = " << data << ", size = " << size  << endl;

        res = nic->nic_receive(src, prot, data, size);

    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for NIC agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_nic_statistics()
{
    Adapter<NIC::Statistics> * nic_statistics = reinterpret_cast<Adapter<NIC::Statistics> *>(id().unit());

    Result res = 0;

    switch (method()) {
    case DESTROY: {
        // delete nic_statistics; /* Adapter object must be destroyed but not "core" object. Decided to not delete at all. */
    } break;
    case NIC_STATISTICS_RX_PACKETS: {
        res = nic_statistics->get_rx_packets();
    } break;
    case NIC_STATISTICS_RX_BYTES: {
        res = nic_statistics->get_rx_bytes();
    } break;
    case NIC_STATISTICS_TX_PACKETS: {
        res = nic_statistics->get_tx_packets();
    } break;
    case NIC_STATISTICS_TX_BYTES: {
        res = nic_statistics->get_tx_bytes();
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for NIC::Statistics Agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_ip()
{
    Adapter<IP> * ip = reinterpret_cast<Adapter<IP> *>(id().unit());
    Result res = 0;

    switch (method()) {
    case IP_NIC: {
        res = reinterpret_cast<Result>(ip->nic());
    } break;
    case IP_ADDRESS: {
        db<Framework>(TRC) << "IP agent, IP_ADDRESS" << endl;

        res = reinterpret_cast<Result>(ip->address());

        db<Framework>(TRC) << "res = " << res << " (" << reinterpret_cast<void *>(res) << ")" << endl;

    } break;
    case IP_GET_BY_NIC: {
        db<Framework>(TRC) << "IP agent, IP_GET_BY_NIC" << endl;

        unsigned int unit;
        in(unit);
        res = reinterpret_cast<Result>(Adapter<IP>::get_by_nic(unit));
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for IP agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}

void Agent::handle_ip_address()
{
    Adapter<IP::Address> * ip_address = reinterpret_cast<Adapter<IP::Address> *>(id().unit());
    Result res = 0;

    switch (method()) {
    case IP_ADDRESS_PRINT: {
        db<Framework>(TRC) << "IP::Address Agent, PRINT" << endl;
        IP::Address * addr;
        in(addr);
        db<void>(WRN) << " " << *addr << endl;
    } break;
    case IP_ADDRESS_ARRAY_SUBSCRIPT: {
        size_t i;
        in(i);
        res = ip_address[i];
    } break;
    case DESTROY: {
        db<Framework>(TRC) << "IP::Address Agent, DESTROY: " << reinterpret_cast<void *>(ip_address) << endl;

        // delete ip_address; /* XXX: the deletion of ip_address is causing a page fault. Investigate that. */
        db<Framework>(TRC) << "IP::Address destroyed" << endl;

    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for IP::Address agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}

void Agent::handle_cpu()
{
    Result res = 0;

    // Adapter<CPU> * cpu = reinterpret_cast<Adapter<CPU> *>(id().unit());

    switch(method()) {
    case CPU_INT_ENABLE: {
        Adapter<CPU>::int_enable();
    } break;
    case CPU_INT_DISABLE: {
        Adapter<CPU>::int_disable();
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for CPU agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_pedf()
{
    Adapter<Scheduling_Criteria::PEDF> * pedf = reinterpret_cast<Adapter<Scheduling_Criteria::PEDF> *>(id().unit());

    Result res = 0;

    switch(method()) {
    case CREATE1: {
        db<Framework>(TRC) << "PEDF Agent, CREATE1" << endl;
        id(Id(PEDF_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Scheduling_Criteria::PEDF>()))); // Scheduling_Criteria::PEDF::APERIODIC
        db<Framework>(TRC) << "PEDF created: " << reinterpret_cast<void *>(id().unit()) << endl;
    } break;
    case CREATE2: {
        db<Framework>(TRC) << "PEDF Agent, CREATE2" << endl;
        int priority;
        int cpu;
        in(priority, cpu);
        id(Id(PEDF_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Scheduling_Criteria::PEDF>(priority, cpu))));
        db<Framework>(TRC) << "PEDF created: " << reinterpret_cast<void *>(id().unit()) << endl;
    } break;
    case CREATE4: {
        db<Framework>(TRC) << "PEDF Agent, CREATE4" << endl;
        RTC::Microsecond deadline;
        RTC::Microsecond period;
        RTC::Microsecond capacity;
        int pcpu;
        in(deadline, period, capacity, pcpu);
        id(Id(PEDF_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Scheduling_Criteria::PEDF>(deadline, period, capacity, pcpu))));
        db<Framework>(TRC) << "PEDF created: " << reinterpret_cast<void *>(id().unit()) << endl;
    } break;
    case DESTROY: {
        db<Framework>(TRC) << "PEDF Agent, DESTROY" << endl;
        delete pedf;
    } break;
    case PRINT: {
        db<Framework>(TRC) << "PRINT PEDF" << endl;
        Scheduling_Criteria::PEDF * p;
        in(p);
        db<void>(WRN) << "PEDF." << *p << endl; /* For some reason this don't work without the string before the **p** object. */
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for PEDF agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_thread_configuration()
{
    Adapter<Thread::Configuration> * conf = reinterpret_cast<Adapter<Thread::Configuration> *>(id().unit());

    Result res = 0;
    switch(method()) {
    case CREATE4: {
        Periodic_Thread::State state;
        Periodic_Thread::Criterion * criterion;
        Task * task;
        unsigned int stack_size;
        in(task, state, stack_size, criterion);
        db<Framework>(TRC) << "Thread::Configuration Agent, CREATE4" << endl;
        db<Framework>(TRC) << ", state = " << state
                            << ", criterion = " << *criterion
                            << ", task = " << reinterpret_cast<void *>(task)
                            // << ", stack_size = " << stack_size
                            << endl;

        db<Framework>(TRC) << "Criterion object = " << reinterpret_cast<void *>(criterion) << endl;

        id(Id(THREAD_CONFIGURATION_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Thread::Configuration>(state
            , *criterion
            , task))));

        db<Framework>(TRC) << "Conf created: " << reinterpret_cast<void *>(id().unit()) << endl;

    } break;
    case DESTROY: {
        db<Framework>(TRC) << "Thread::Configuration Agent, DESTROY" << endl;
        delete conf;
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Thread::Configuration agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }
    result(res);
}


void Agent::handle_periodic_thread_configuration()
{
    Adapter<Periodic_Thread::Configuration> * conf = reinterpret_cast<Adapter<Periodic_Thread::Configuration> *>(id().unit());

    Result res = 0;
    switch(method()) {
    case CREATE6: {
        RTC::Microsecond period;
        int times;
        Periodic_Thread::State state;
        Periodic_Thread::Criterion * criterion;
        Task * task;
        unsigned int stack_size;
        in(task, period, times, state, stack_size, criterion);
        db<Framework>(TRC) << "Periodic_Thread::Configuration Agent, CREATE6" << endl;
        db<Framework>(TRC) << "period = " << period
                            << ", times = " << times
                            << ", state = " << state
                            << ", criterion = " << *criterion
                            << ", task = " << reinterpret_cast<void *>(task)
                            << ", stack_size = " << stack_size << endl;

        db<Framework>(TRC) << "PEDF object = " << reinterpret_cast<void *>(criterion) << endl;

        id(Id(PERIODIC_THREAD_CONFIGURATION_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Periodic_Thread::Configuration>(period
            , times
            , state
            , *criterion
            , task
            , stack_size))));

        db<Framework>(TRC) << "Conf created: " << reinterpret_cast<void *>(id().unit()) << endl;

    } break;
    case DESTROY: {
        db<Framework>(TRC) << "Periodic_Thread::Configuration Agent, DESTROY" << endl;
        delete conf;
    } break;
    case PRINT: {
        db<Framework>(TRC) << "Periodic_Thread::Configuration Agent, PRINT" << endl;
        Periodic_Thread::Configuration * conf;
        in(conf);
        db<void>(WRN) << "Conf." << *conf << endl;

    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Periodic_Thread::Configuration agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }
    result(res);
}


void Agent::handle_tcp_link()
{
    Adapter<TCP_Link> * link = reinterpret_cast<Adapter<TCP_Link> *>(id().unit());

    Result res = 0;

    switch(method()) {
    case CREATE1: {
        TCP::Port local;
        in(local);

        db<Framework>(TRC) << "Agent: Creating TCP Link. local = " << local << endl;

        id(Id(TCP_LINK_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<TCP_Link>(local))));

    } break;
    case DESTROY: {
        delete link;
    } break;
    case TCP_LINK_READ: {
        void * data;
        unsigned int size;

        in(data, size);

        db<Framework>(TRC) << "Agent: TCP_Link read. data = " << data << ", size = " << size << endl;

        res = link->tcp_link_read(data, size);
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for TCP Link agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}

void Agent::handle_ether_channel_link()
{
    Adapter<Ether_Channel_Link> * link = reinterpret_cast<Adapter<Ether_Channel_Link> *>(id().unit());

    Result res = 0;

    switch(method()) {
    case CREATE1: {
        db<Framework>(TRC) << "Agent: Creating Ether_Channel Link. " << endl;

        id(Id(ETHER_CHANNEL_LINK_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Ether_Channel_Link>())));

    } break;
    case DESTROY: {
        delete link;
    } break;
    case ETHER_CHANNEL_LINK_READ: {
        void * data;
        unsigned int size;

        in(data, size);

        db<Framework>(TRC) << "Agent: Ether_Channel_Link read. data = " << data << ", size = " << size << endl;

        res = link->ether_channel_link_read(data, size);
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Ether_Channel Link agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}

void Agent::handle_mmu_aux()
{
    Result res = 0;

    switch(method()) {
    case MMU_AUX_PHYSICAL_ADDRESS: {
        unsigned long log_addr;
        unsigned long * out_page_frame_present;
        in(log_addr, out_page_frame_present);

        db<Framework>(TRC) << "MMU_Aux agent, MMU_AUX_PHYSICAL_ADDRESS. log_addr = "
                            << reinterpret_cast<void *>(log_addr)
                            << ", out_page_frame_present = " << reinterpret_cast<void *>(out_page_frame_present)
                            << endl;

        res = Adapter<MMU_Aux>::physical_address(log_addr, out_page_frame_present);

        db<Framework>(TRC) << "phy_addr = "
            << reinterpret_cast<void *>(res)
            << ", page_frame_present = " << out_page_frame_present
            << endl;

    } break;
    case MMU_AUX_DUMP_MEMORY_MAPPING: {
        Adapter<MMU_Aux>::dump_memory_mapping();

    } break;
    case MMU_AUX_CHECK_MEMORY_MAPPING: {
        Adapter<MMU_Aux>::check_memory_mapping();

    } break;
    case MMU_AUX_SET_AS_READ_ONLY: {
        unsigned long log_addr;
        unsigned long size;
        bool user;

        in(log_addr, size, user);

        db<Framework>(TRC) << Color::GREEN()
            << "log_addr = " << reinterpret_cast<void *>(log_addr)
            << ", size = " << size
            << ", user = " << user
            << Color::END_COLOR()
            << endl;

        Adapter<MMU_Aux>::set_as_read_only(log_addr, size, user);

    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for MMU_Aux agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}

void Agent::handle_tsc()
{
    Result res = 0;

    switch(method()) {
    case TSC_TIME_STAMP: {
        res = Adapter<TSC>::time_stamp();

        if (res < 0) {
            db<Framework>(WRN) << "TSC_TIME_STAMP. res = " << res << endl;
            db<Framework>(ERR) << "Error Time_Stamp overflow!" << endl;
        }

    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for TSC agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}

void Agent::handle_chronometer_aux()
{
    Result res = 0;

    switch(method()) {
    case CHRONO_ELAPSED_NANO: {
        TSC::Time_Stamp start;
        TSC::Time_Stamp stop;
        in(start, stop);

        res = Adapter<Chronometer_Aux>::elapsed_nano(start, stop);

        db<Framework>(TRC) << "CHRONO_ELAPSED_NANO, start = " << start << ", stop = " << stop << ", res = " << res << endl;

    } break;
    case CHRONO_ELAPSED_MICRO: {
        TSC::Time_Stamp start;
        TSC::Time_Stamp stop;
        in(start, stop);

        res = Adapter<Chronometer_Aux>::elapsed_micro(start, stop);

        db<Framework>(TRC) << "CHRONO_ELAPSED_MICRO, start = " << start << ", stop = " << stop << ", res = " << res << endl;

    } break;
    case CHRONO_ELAPSED_SEC: {
        TSC::Time_Stamp start;
        TSC::Time_Stamp stop;
        in(start, stop);

        res = Adapter<Chronometer_Aux>::elapsed_sec(start, stop);

        db<Framework>(TRC) << "CHRONO_ELAPSED_SEC, start = " << start << ", stop = " << stop << ", res = " << res << endl;

    } break;
    case CHRONO_NANO: {
        TSC::Time_Stamp ticks;
        in(ticks);

        res = Adapter<Chronometer_Aux>::nano(ticks);

        db<Framework>(TRC) << "CHRONO_NANO, ticks = " << ticks << ", res = " << res << endl;

    } break;
    case CHRONO_MICRO: {
        TSC::Time_Stamp ticks;
        in(ticks);

        res = Adapter<Chronometer_Aux>::micro(ticks);

        db<Framework>(TRC) << "CHRONO_MICRO, ticks = " << ticks << ", res = " << res << endl;

    } break;
    case CHRONO_SEC: {
        TSC::Time_Stamp ticks;
        in(ticks);

        res = Adapter<Chronometer_Aux>::sec(ticks);

        db<Framework>(TRC) << "CHRONO_SEC, ticks = " << ticks << ", res = " << res << endl;

    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for Chronometer_Aux agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_fpga()
{
    Result res = 0;

    switch(method()) {
    case FPGA_RUN: {
        Adapter<FPGA>::run();
    } break;
    case FPGA_REPORT: {
        Adapter<FPGA>::report();
    } break;
    case FPGA_PRINT_CONFIGURATION: {
        Adapter<FPGA>::print_configuration();
    } break;
    case FPGA_MONITOR_START: {
        Adapter<FPGA>::monitor_start();
    } break;
    case FPGA_MONITOR_STOP: {
        Adapter<FPGA>::monitor_stop();
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for FPGA agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_uart()
{
    Adapter<UART> * uart = reinterpret_cast<Adapter<UART> *>(id().unit());

    Result res = 0;

    switch(method()) {
    case CREATE: {
        // db<void>(WRN) << "UART Agent, CREATE" << endl;

        id(Id(UART_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<UART>())));

    } break;
    case DESTROY: {
        delete uart;
    } break;
    case UART_RTS_DOWN: {
        // db<void>(WRN) << "UART Agent, RTS DOWN" << endl;
        uart->rts_down();
    } break;
    case UART_RTS_UP: {
        // db<void>(WRN) << "UART Agent, RTS UP" << endl;
        uart->rts_up();
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for UART agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}


void Agent::handle_utility()
{
    Result res = 0;

    switch(method()) {
    case PRINT: {
        const char * s;
        in(s);
        _print(s);
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for UTILITY agent. Method = " << method() << endl;
        res = UNDEFINED;
    }
    }

    result(res);
}

__END_SYS

#endif
