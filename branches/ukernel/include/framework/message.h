// EPOS Component Framework - Proxy x Agent Message

#ifndef __message_h
#define __message_h

#include "id.h"

extern "C" { void _syscall(void *); }

__BEGIN_SYS

class Message
{
public:
    static const unsigned int MAX_PARAMETERS_SIZE = 52;

    enum {
        CREATE,
        CREATE1,
        CREATE2,
        CREATE3,
        CREATE4,
        CREATE5,
        CREATE6,
        CREATE7,
        CREATE8,
        CREATE9,
        DESTROY,
        SELF,

        COMPONENT = 0x10,

        THREAD_STATE = COMPONENT,
        THREAD_PRIORITY,
        THREAD_PRIORITY1,
        THREAD_JOIN,
        THREAD_PASS,
        THREAD_SUSPEND,
        THREAD_RESUME,
        THREAD_YIELD,
        THREAD_EXIT,
        THREAD_WAIT_NEXT,

        TASK_ADDRESS_SPACE = COMPONENT,
        TASK_CODE_SEGMENT,
        TASK_DATA_SEGMENT,
        TASK_CODE,
        TASK_DATA,
        TASK_MAIN,
        TASK_SET_MAIN,

        ADDRESS_SPACE_PD = COMPONENT,
        ADDRESS_SPACE_ATTACH1,
        ADDRESS_SPACE_ATTACH2,
        ADDRESS_SPACE_DETACH,
        ADDRESS_PHYSICAL,

        SEGMENT_SIZE = COMPONENT,
        SEGMENT_PHY_ADDRESS,
        SEGMENT_RESIZE,

        SYNCHRONIZER_LOCK = COMPONENT,
        SYNCHRONIZER_UNLOCK,
        SYNCHRONIZER_P,
        SYNCHRONIZER_V,
        SYNCHRONIZER_WAIT,
        SYNCHRONIZER_SIGNAL,
        SYNCHRONIZER_BROADCAST,

        ALARM_DELAY = COMPONENT,

        COMMUNICATOR_SEND = COMPONENT,
        COMMUNICATOR_RECEIVE,

        NETWORK_INIT = COMPONENT,

        NIC_STATISTICS = COMPONENT,

        IP_GET_BY_NIC = COMPONENT,
        IP_ADDRESS,
        IP_NIC,

        PRINT = COMPONENT,

        MACHINE_SMP_BARRIER = COMPONENT,
        MACHINE_CPU_ID,

        THIS_THREAD_ID_ID = COMPONENT,

        CPU_INT_ENABLE = COMPONENT,
        CPU_INT_DISABLE,

        UNDEFINED = -1
    };
    typedef int Method;
    typedef Method Result;

    typedef Simple_List<Message> List;
    typedef List::Element Element;

public:
    Message(): _link(this) {}
    Message(const Message & msg): _id(msg.id()), _method(msg.method()), _link(this) { memcpy(_parms, msg._parms, MAX_PARAMETERS_SIZE); }
    Message(const Id & id): _id(id), _link(this) {}
    template<typename ... Tn>
    Message(const Id & id, const Method & m, Tn && ... an): _id(id), _method(m), _link(this) { out(an ...); }

    const Id & id() const { return _id; }
    void id(const Id & id) { _id = id; }

    const Method & method() const { return _method; }
    void method(const Method & m) { _method = m; }
    const Result & result() const { return _method; }
    void result(const Result & r) { _method = r; }

    template<typename ... Tn>
    void in(Tn && ... an) {
        // Force a compilation error in case out is called with too many arguments
        typename IF<(SIZEOF<Tn ...>::Result <= MAX_PARAMETERS_SIZE), int, void>::Result index = 0;
        DESERIALIZE(_parms, index, an ...);
    }
    template<typename ... Tn>
    void out(const Tn & ... an) {
        // Force a compilation error in case out is called with too many arguments
        typename IF<(SIZEOF<Tn ...>::Result <= MAX_PARAMETERS_SIZE), int, void>::Result index = 0;
        SERIALIZE(_parms, index, an ...);
    }

    void act() { _syscall(this); }

    Element * lext() { return &_link; }

    friend Debug & operator << (Debug & db, const Message & m) {
          db << "{id=" << m._id << ",m=" << hex << m._method
             << ",p={" << reinterpret_cast<void *>(*static_cast<const int *>(reinterpret_cast<const void *>(&m._parms[0]))) << ","
             << reinterpret_cast<void *>(*static_cast<const int *>(reinterpret_cast<const void *>(&m._parms[4]))) << ","
             << reinterpret_cast<void *>(*static_cast<const int *>(reinterpret_cast<const void *>(&m._parms[8]))) << "}}";
          return db;
      }

private:
    Id _id;
    Method _method;
    char _parms[MAX_PARAMETERS_SIZE];

    Element _link;
};

__END_SYS

#endif
