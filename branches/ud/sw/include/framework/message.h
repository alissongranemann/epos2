// EPOS Component Framework - Proxy x Agent Message

#ifndef __message_h
#define __message_h

#include "id.h"
#include "serializer.h"

__BEGIN_SYS

class Message_Common
{
protected:
    static const unsigned int MAX_PARAMETERS_SIZE = 20;

public:
    enum Methods {
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

        ADDER_ADD = COMPONENT,
        ADDER_REST_ST,
        ADDER_SAVE_ST,
        ADDER_GET_ST_LEN,

        PRINT = COMPONENT,

        UNDEFINED = -1
    };
    typedef int Method;
    typedef Method Result;

public:
    Message_Common() {}
    Message_Common(const Id & id): _id(id) {}

    const Id & id() const { return _id; }
    void id(const Id & id) { _id = id; }

    const Method & method() const { return _method; }
    void result(const Result & r) { _method = r; }

protected:
    Id _id;
    Method _method;
};

__END_SYS

#endif
