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

        if (id().type() != UTILITY_ID)
            db<Framework>(TRC) << ":=>" << *reinterpret_cast<Message *>(this) << endl;

        if (id().type() < LAST_TYPE_ID) { // in-kernel services
            if (_handlers[id().type()]) {
                (this->*_handlers[id().type()])();
            }
            else {
                db<Framework>(ERR) << "Error: handler for type " << id().type() << " not registered." << endl;
            }
        }
        else { // out-of-kernel (i.e. Dom0 or server) services
                Message msg(*this); // copy message from user space to kernel
                msg.id(Id(IPC_COMMUNICATOR_ID, id().unit()));
                if (IPC::send(id().unit(), &msg)) { // 0 => no one listening
                    Port<IPC> * comm = reinterpret_cast<Port<IPC> *>(IPC::observer(id().type())); // recall the Port<IPC> that got us here
                    comm->receive(this); // copy from kernel to user
                } else
                    result(UNDEFINED);
        }

        if (id().type() != UTILITY_ID)
            db<Framework>(TRC) << "<=:" << *reinterpret_cast<Message *>(this) << endl;

        if (result() == UNDEFINED) {
            db<Framework>(ERR) << "exec. Undefined method for Component = " << id().type() << ", method = " << method() << endl;
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
    void handle_ip();
    void handle_cpu();
    void handle_pedf();
    void handle_periodic_thread();
    void handle_thread_configuration();
    void handle_periodic_thread_configuration();

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
        db<Framework>(WRN) << "Thread Agent, CREATE1" << endl;
        int (*entry)();
        in(entry);
        id(Id(THREAD_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Thread>(Thread::Configuration(Thread::READY, Thread::NORMAL, 0, 0), entry))));
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
    case THREAD_WAIT_NEXT: {
        //            Periodic_Thread::wait_next();
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

    Result res = 0;

    switch(method()) {
    case CREATE2: {
        db<Framework>(WRN) << "Agent::handle_periodic_thread, CREATE2" << endl;
        Periodic_Thread::Configuration * conf;
        int (* entry)();
        in(conf, entry);

        db<Framework>(TRC) << "conf (obj) = " << conf << endl;
        db<Framework>(TRC) << "conf = " << *conf << endl;

        id(Id(PERIODIC_THREAD_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Periodic_Thread>(*conf, entry))));

        db<Framework>(TRC) << "created = " << reinterpret_cast<void *>(id().unit()) << endl;

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
        db<Framework>(WRN) << "Task Agent, CREATE3" << endl;
        Segment * cs, * ds;
        int (*entry)();
        in(cs, ds, entry);
        id(Id(TASK_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Task>(cs, ds, entry))));
    } break;
    case CREATE4: {
        db<Framework>(WRN) << "Task Agent, CREATE4" << endl;
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
        db<Framework>(WRN) << "Address Space Agent, CREATE" << endl;
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
        res = as->attach(seg);
    } break;
    case ADDRESS_SPACE_ATTACH2: {
        Segment * seg;
        CPU::Log_Addr addr;
        in(seg, addr);
        res = as->attach(seg, addr);
    } break;
    case ADDRESS_SPACE_DETACH: {
        Segment * seg;
        in(seg);
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
        db<Framework>(WRN) << "Segment Agent, CREATE1" << endl;
        unsigned int bytes;
        in(bytes);
        id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Segment>(bytes))));
    } break;
    case CREATE2: { // *** indistinguishable ***
        db<Framework>(WRN) << "Segment Agent, CREATE2" << endl;
        unsigned int bytes;
        Segment::Flags flags;
        in(bytes, flags);
        id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Segment>(bytes, flags))));
    } break;
    case CREATE3: { // *** indistinguishable ***
        db<Framework>(WRN) << "Segment Agent, CREATE3" << endl;
        Segment::Phy_Addr phy_addr;
        unsigned int bytes;
        Segment::Flags flags;
        in(phy_addr, bytes, flags);
        id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Segment>(phy_addr, bytes, flags))));
    } break;
    case DESTROY: {
        delete seg;
    } break;
    case SEGMENT_SIZE: {
        res = seg->size();
    } break;
    case SEGMENT_PHY_ADDRESS: {
        res = seg->phy_address();
    } break;
    case SEGMENT_RESIZE: {
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
        db<void>(TRC) << "place: " << reinterpret_cast<void *>(place) << ", heap_segment: " << reinterpret_cast<void *>(heap_segment) << endl;
        db<void>(TRC) << "current task is: " << Task::current() << endl;
        db<void>(TRC) << "address space is: " << Task::current()->address_space() << endl;
        db<void>(TRC) << "page directory is: " << Task::current()->address_space()->pd() << endl;
        db<void>(TRC) << "segment size: " << heap_segment->size() << endl;
        CPU::Log_Addr addr = Task::current()->address_space()->attach(heap_segment);
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
        db<Framework>(WRN) << "Alarm Agent, CREATE3" << endl;
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


//void Agent::handle_communicator()
//{
//    Adapter<Port<IPC>> * comm = reinterpret_cast<Adapter<Port<IPC>> *>(id().unit());
//    Result res = 0;
//
//    switch(method()) {
//    case CREATE1: {
//        Port<IPC>::Local_Address local;
//        in(local);
//        id(Id(COMMUNICATOR_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Port<IPC>>(local))));
//
//        if((local == DOM0_ID) && !_dom0)
//            _dom0 = reinterpret_cast<Port<IPC> *>(id().unit());
//    } break;
//    case DESTROY: {
//        delete comm;
//    } break;
//    case COMMUNICATOR_SEND: {
//        IPC::Address to;
//        void * data;
//        unsigned int size;
//        in(to, data, size);
//        comm->send(to, data, size);
//    } break;
//    case COMMUNICATOR_RECEIVE: {
//        IPC::Address from;
//        void * data;
//        unsigned int size;
//        in(from, data, size);
//        comm->receive(&from, data, size);
//        out(from);
//    } break;
//    case COMMUNICATOR_BIND: {
//        IPC::Address addr;
//        Port<IPC> * comm;
//        in(addr, comm);
//        _dom0 = comm;
//    } break;
//    default: {
//        db<Framework>(WRN) << "Undefined method for Communicator agent. Method = " << method() << endl;
//        res = UNDEFINED;
//    }
//    }
//}


void Agent::handle_ipc()
{
    Adapter<Port<IPC>> * comm = reinterpret_cast<Adapter<Port<IPC>> *>(id().unit());
    Result res = 0;

    switch(method()) {
    case CREATE1: {
        db<Framework>(WRN) << "IPC Agent, CREATE1" << endl;
        Port<IPC>::Local_Address local;
        in(local);
        id(Id(IPC_COMMUNICATOR_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Port<IPC>>(local))));
    } break;
    case DESTROY: {
        delete comm;
    } break;
    case COMMUNICATOR_SEND: {
        Message * msg;
        in(msg);
        comm->send(msg);
    } break;
    case COMMUNICATOR_RECEIVE: {
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
        Adapter<Network>::init_network();
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
    case NIC_STATISTICS: {
        res = reinterpret_cast<Result>(nic->statistics());
    } break;
    default: {
        db<Framework>(WRN) << "Undefined method for NIC agent. Method = " << method() << endl;
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
        res = reinterpret_cast<Result>(ip->address());
    } break;
    case IP_GET_BY_NIC: {
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
    case CREATE4: {
        db<Framework>(WRN) << "PEDF Agent, CREATE4" << endl;
        RTC::Microsecond deadline;
        RTC::Microsecond period;
        RTC::Microsecond capacity;
        int pcpu;
        in(deadline, period, capacity, pcpu);
        id(Id(PEDF_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Scheduling_Criteria::PEDF>(deadline, period, capacity, pcpu))));
        db<Framework>(WRN) << "PEDF created: " << reinterpret_cast<void *>(id().unit()) << endl;
    } break;
    case DESTROY: {
        db<Framework>(WRN) << "PEDF Agent, DESTROY" << endl;
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
    Result res = 0;
    switch(method()) {
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
        db<Framework>(WRN) << "Periodic_Thread::Configuration Agent, CREATE6" << endl;
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
        db<Framework>(WRN) << "Periodic_Thread::Configuration Agent, DESTROY" << endl;
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
