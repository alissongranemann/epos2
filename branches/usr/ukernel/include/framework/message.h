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

        PERIODIC_THREAD_WAIT_NEXT = COMPONENT,

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
        CREATE_SEGMENT_IN_PLACE,
        CREATE_HEAP_IN_PLACE,

        SYNCHRONIZER_LOCK = COMPONENT,
        SYNCHRONIZER_UNLOCK,
        SYNCHRONIZER_P,
        SYNCHRONIZER_V,
        SYNCHRONIZER_WAIT,
        SYNCHRONIZER_SIGNAL,
        SYNCHRONIZER_BROADCAST,

        ALARM_DELAY = COMPONENT,

        IPC_SEND = COMPONENT,
        IPC_RECEIVE,

        NETWORK_INIT = COMPONENT,

        NIC_STATISTICS = COMPONENT,
        NIC_ADDRESS,
        NIC_ADDRESS_PRINT,
        NIC_MTU,
        NIC_RECEIVE,

        NIC_STATISTICS_TX_PACKETS = COMPONENT,
        NIC_STATISTICS_TX_BYTES,
        NIC_STATISTICS_RX_PACKETS,
        NIC_STATISTICS_RX_BYTES,

        IP_GET_BY_NIC = COMPONENT,
        IP_ADDRESS,
        IP_NIC,

        IP_ADDRESS_PRINT = COMPONENT,
        IP_ADDRESS_ARRAY_SUBSCRIPT,
        IP_ADDRESS_ARRAY_SUBSCRIPT_CONST,

        TCP_LINK_READ = COMPONENT,

        ETHER_CHANNEL_LINK_READ = COMPONENT,

        PRINT = COMPONENT,

        MACHINE_SMP_BARRIER = COMPONENT,
        MACHINE_CPU_ID,

        THIS_THREAD_ID_ID = COMPONENT,

        CPU_INT_ENABLE = COMPONENT,
        CPU_INT_DISABLE,

        MMU_AUX_PHYSICAL_ADDRESS = COMPONENT,
        MMU_AUX_DUMP_MEMORY_MAPPING,
        MMU_AUX_CHECK_MEMORY_MAPPING,
        MMU_AUX_SET_AS_READ_ONLY,

        TSC_TIME_STAMP = COMPONENT,

        CHRONO_ELAPSED_NANO = COMPONENT,
        CHRONO_ELAPSED_MICRO,
        CHRONO_ELAPSED_SEC,
        CHRONO_NANO,
        CHRONO_MICRO,
        CHRONO_SEC,

        FPGA_RUN = COMPONENT,
        FPGA_REPORT,
        FPGA_PRINT_CONFIGURATION,

        UART_RTS_DOWN = COMPONENT,
        UART_RTS_UP,

        UNDEFINED = -1
    };
    typedef long long Method;
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
