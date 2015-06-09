// EPOS Component Framework - Component Agent

#ifndef __agent_h
#define __agent_h

#include "message.h"

__BEGIN_SYS

// EPOS Component Framework - Component Agent

template<typename Component>
class Agent: public Message
{
public:
    void exec() { result(UNDEFINED); }
};


#include <thread.h>
//#include <periodic_thread.h>
template<>
class Agent<Thread>: public Message
{
public:
    void exec() {
        Adapter<Thread> * thread = reinterpret_cast<Adapter<Thread> *>(id().unit());
        Result res = 0;

        switch(method()) {
        case CREATE1: {
            int (*entry)();
            in(entry);
            id(Id(THREAD_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Thread>(Thread::Configuration(Thread::READY, Thread::NORMAL, 0, 0), entry))));
        }
        break;
        case CREATE2: {
            Adapter<Task> * task;
            int (*entry)();
            in(task, entry);
            id(Id(THREAD_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Thread>(Thread::Configuration(Thread::READY, Thread::NORMAL, task, 0), entry))));
        }
        break;
        case DESTROY:
            delete thread;
            break;
        case SELF:
            id(Id(THREAD_ID, reinterpret_cast<Id::Unit_Id>(Adapter<Thread>::self())));
            break;
        case THREAD_SUSPEND:
            thread->suspend();
            break;
        case THREAD_RESUME:
            thread->resume();
            break;
        case THREAD_JOIN:
            res = thread->join();
            break;
        case THREAD_PASS:
            thread->pass();
            break;
        case THREAD_YIELD:
            Thread::yield();
            break;
        case THREAD_WAIT_NEXT:
            //            Periodic_Thread::wait_next();
            break;
        case THREAD_EXIT: {
            int r;
            in(r);
            Thread::exit(r);
        }
        default:
            res = UNDEFINED;
            break;
        }
        result(res);
    }
};


#include <task.h>
template<>
class Agent<Task>: public Message
{
public:
    void exec() {
        Adapter<Task> * task = reinterpret_cast<Adapter<Task> *>(id().unit());
        Result res = 0;

        switch(method()) {
        case CREATE3: {
            Segment * cs, * ds;
            int (*entry)();
            in(cs, ds, entry);
            id(Id(TASK_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Task>(cs, ds, entry))));
        }
        break;
        case DESTROY:
            delete task;
            break;
        case SELF:
            id(Id(TASK_ID, reinterpret_cast<Id::Unit_Id>(Adapter<Task>::self())));
            break;
        case TASK_ADDRESS_SPACE:
            res = reinterpret_cast<int>(task->address_space());
            break;
        case TASK_CODE_SEGMENT:
            res = reinterpret_cast<int>(task->code_segment());
            break;
        case TASK_DATA_SEGMENT:
            res = reinterpret_cast<int>(task->data_segment());
            break;
        case TASK_CODE:
            res = task->code();
            break;
        case TASK_DATA:
            res = task->data();
            break;
        case TASK_MAIN:
            res = reinterpret_cast<int>(task->main());
            break;
        default:
            res = UNDEFINED;
            break;
        }
        result(res);
    }
};


#include <address_space.h>
template<>
class Agent<Address_Space>: public Message
{
public:
    void exec() {
        Adapter<Address_Space> * as = reinterpret_cast<Adapter<Address_Space> *>(id().unit());
        Result res = 0;

        switch(method()) {
        case CREATE:
            id(Id(ADDRESS_SPACE_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Address_Space>())));
            break;
        case DESTROY:
            delete as;
            break;
        case ADDRESS_SPACE_PD:
            res = as->pd();
            break;
        case ADDRESS_SPACE_ATTACH1: {
            Segment * seg;
            in(seg);
            res = as->attach(seg);
        }
        break;
        case ADDRESS_SPACE_ATTACH2: {
            Segment * seg;
            CPU::Log_Addr addr;
            in(seg, addr);
            res = as->attach(seg, addr);
        }
        break;
        case ADDRESS_SPACE_DETACH: {
            Segment * seg;
            in(seg);
            as->detach(seg);
        }
        break;
        case ADDRESS_PHYSICAL: {
            CPU::Log_Addr addr;
            in(addr);
            res = as->physical(addr);
        }
        break;
        default:
            res = UNDEFINED;
            break;

        }
        result(res);
    }
};


#include <segment.h>
template<>
class Agent<Segment>: public Message
{
public:
    void exec() {
        Adapter<Segment> * seg = reinterpret_cast<Adapter<Segment> *>(id().unit());
        Result res = 0;

        switch(method()) {
        case CREATE1: {
            unsigned int bytes;
            in(bytes);
            id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Segment>(bytes))));
        }
        break;
        case CREATE2: { // *** indistinguishable ***
            unsigned int bytes;
            Segment::Flags flags;
            in(bytes, flags);
            id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Segment>(bytes, flags))));
        }
        break;
        case CREATE3: { // *** indistinguishable ***
            Segment::Phy_Addr phy_addr;
            unsigned int bytes;
            Segment::Flags flags;
            in(phy_addr, bytes, flags);
            id(Id(SEGMENT_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Segment>(phy_addr, bytes, flags))));
        }
        break;
        case DESTROY:
            delete seg;
            break;
        case SEGMENT_SIZE:
            res = seg->size();
            break;
        case SEGMENT_PHY_ADDRESS:
            res = seg->phy_address();
            break;
        case SEGMENT_RESIZE: {
            int amount;
            in(amount);
            res = seg->resize(amount);
        }
        break;
        default:
            res = UNDEFINED;
            break;

        }
        result(res);
    }
};


#include <alarm.h>
template<>
class Agent<Alarm>: public Message
{
public:
    void exec() {
        Adapter<Alarm> * alarm = reinterpret_cast<Adapter<Alarm> *>(id().unit());
        Result res = 0;

        switch(method()) {
        case DESTROY:
            delete alarm;
            break;
        case CREATE3: {
            Alarm::Microsecond time;
            Handler * handler;
            int times;
            in(time, handler, times);
            id(Id(ALARM_ID, reinterpret_cast<Id::Unit_Id>(new Adapter<Alarm>(time, handler, times))));
        }
        break;
        case ALARM_DELAY: {
            Alarm::Microsecond time;
            in(time);
            Adapter<Alarm>::delay(time);
        }
        break;
        default:
            res = UNDEFINED;
            break;
        }
        result(res);
    }
};


template<>
class Agent<Utility>: public Message
{
public:
    void exec() {
        Result res = 0;

        switch(method()) {
        case PRINT: {
            const char * s;
            in(s);
            _print(s);
        }
        break;
        default:
            res = UNDEFINED;
            break;
        }
        result(res);
    }
};


template<>
class Agent<void>: public Message
{
public:
    void exec() {
        if(id().type() != UTILITY_ID)
            db<Framework>(TRC) << ":=>" << *reinterpret_cast<Message *>(this) << endl;

        switch(id().type()) {
        case THREAD_ID:         reinterpret_cast<Agent<Thread>          *>(this)->exec(); break;
        case TASK_ID:           reinterpret_cast<Agent<Task>            *>(this)->exec(); break;
        case ADDRESS_SPACE_ID:  reinterpret_cast<Agent<Address_Space>   *>(this)->exec(); break;
        case SEGMENT_ID:        reinterpret_cast<Agent<Segment>         *>(this)->exec(); break;
        case ALARM_ID:          reinterpret_cast<Agent<Alarm>           *>(this)->exec(); break;
        case UTILITY_ID:        reinterpret_cast<Agent<Utility>         *>(this)->exec(); break;
        default:                result(Message::UNDEFINED);
        }

        if(id().type() != UTILITY_ID)
            db<Framework>(TRC) << "<=:" << *reinterpret_cast<Message *>(this) << endl;
    }
};

__END_SYS

#endif
