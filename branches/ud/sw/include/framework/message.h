// EPOS Component Framework - Proxy x Agent Message

#ifndef __message_h
#define __message_h

#include "id.h"

extern "C" { int _syscall(void *); }

__BEGIN_SYS

class Message
{
private:
    static const unsigned int MAX_PARAMETERS_SIZE = 20;

public:
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
        DESTROY,
        SELF,
        COMPONENT = 0x10,

        THREAD_SUSPEND = COMPONENT,
        THREAD_RESUME,
        THREAD_JOIN,
        THREAD_PASS,
        THREAD_YIELD,
        THREAD_EXIT,
        THREAD_WAIT_NEXT,

        TASK_ADDRESS_SPACE = COMPONENT,
        TASK_CODE_SEGMENT,
        TASK_DATA_SEGMENT,
        TASK_CODE,
        TASK_DATA,

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

        PRINT = COMPONENT,

        UNDEFINED = -1
    };
    typedef int Method;
    typedef Method Result;

public:
    Message() {}
    Message(const Id & id): _id(id) {}

    const Id & id() const { return _id; }
    void id(const Id & id) { _id = id; }

    const Method & method() const { return _method; }
    void result(const Result & r) { _method = r; }

    template<typename ... Tn>
    int act(const Method & m, const Tn & ... an) {
        _method = m;
        out(an ...);
        _syscall(this);
        return _method;
    }

    template<typename ... Tn>
    void in(Tn && ... an) {
        // Force a compilation error in case out is called with too many parameters
        typename IF<(SIZEOF<Tn ...>::Result <= MAX_PARAMETERS_SIZE), int, void>::Result index = 0;
        DESERIALIZE(_parms, index, an ...);
    }
    template<typename ... Tn>
    void out(const Tn & ... an) {
        // Force a compilation error in case out is called with too many parameters
        typename IF<(SIZEOF<Tn ...>::Result <= MAX_PARAMETERS_SIZE), int, void>::Result index = 0;
        SERIALIZE(_parms, index, an ...);
    }

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
};

__END_SYS

#endif
