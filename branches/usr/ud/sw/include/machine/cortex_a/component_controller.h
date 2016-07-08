// EPOS Cortex-A Component Controller Mediator

#ifndef __cortex_a_component_controller_h
#define __cortex_a_component_controller_h

#include <cpu.h>
#include <ic.h>
#include <machine.h>
#include <component_controller.h>

#include "../../../../unified/rtsnoc.h"

__BEGIN_SYS

class Cortex_A_Component_Controller: public Component_Controller_Common
{
private:
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Reg32 Reg32;
    typedef RTSNoC::Address Address;

public:
    // Error return result
    enum {
        CMD_RESULT_ERR  = 0xFFFFFFFF
    };

    struct Call_Info {
        unsigned int disp_addr;
        unsigned int obj_addr;
    };

    // Internal buffers used to implement RTSNoC's handshake
    class Buffer
    {
        friend class Cortex_A_Component_Controller;

    public:
        // Proxy
        Buffer(unsigned int type, unsigned int unit, Address addr): _type(type),
                _unit(unit), _addr(addr) {
            if(!alloc_proxy(this)) {
                db<Component_Controller>(ERR) << "Couldn't allocate proxy" << endl;
                Machine::panic();
            }
        }

        // Agent
        Buffer(unsigned int type, unsigned int unit, Call_Info info): _type(type),
                _unit(unit), _info(info) {
            if(!alloc_agent(this)) {
                db<Component_Controller>(ERR) << "Couldn't allocate agent" << endl;
                Machine::panic();
            }
        }

        ~Buffer() { free_buf(this); }

        friend Debug & operator << (Debug & db, const Buffer & b) {
            db << "{x=" << b._addr._x << ",y=" << b._addr._y << ",local="
                << b._addr._h << ",type=" << b._type << ",unit=" << b._unit
                << ",disp_addr=" << reinterpret_cast<void *>(b._info.disp_addr)
                << ",obj_addr=" << reinterpret_cast<void *>(b._info.obj_addr)
                << "}";
            return db;
        }

        const unsigned int & id() const { return _id; }
        const unsigned int & type() const { return _type; }
        const unsigned int & unit() const { return _unit; }

    private:
        volatile Reg32 & reg(unsigned int o) { return buf(o | (_id<<2)); }

    private:
        unsigned int _id;
        unsigned int _type;
        unsigned int _unit;
        Call_Info _info;
        Address _addr;
    };

public:
    static bool alloc_proxy(Buffer * buf);
    static bool alloc_agent(Buffer * buf);
    static bool free_buf(Buffer * buf);

    static unsigned int receive_call(Buffer * buf);
    static unsigned int receive_call_data(Buffer * buf);
    static void receive_return_data(Buffer * buf, unsigned int n_ret,
            unsigned long * data);

    static void send_call (Buffer * buf, unsigned int op_id);
    static void send_call_data(Buffer * buf, unsigned int n_args,
            unsigned long * data);
    static void send_return_data(Buffer * buf, unsigned int n_ret,
            unsigned long * data);

    //static bool agent_has_call(agent_call_info &info);
    static void enable_agent_receive_int(IC::Interrupt_Handler h);
    static void disable_agent_receive_int();

private:
    // Base addresses for each register group
    enum {
        CTRL_BASE   = 0x43C00000,
        BUF_BASE    = CTRL_BASE | 0x00000200
    };

    // Control registers offsets
    enum {
        CMD         = 0x00,
        RESULT      = 0x04,
        IDLE        = 0x08,
        AGENT_INT   = 0x0C,
        AGENT_BUF   = 0x10
    };

    // Buffer registers offsets
    enum {
        ADDR_X      = 0x0 << 5,
        ADDR_Y      = 0x1 << 5,
        ADDR_LOCAL  = 0x2 << 5,
        MSG_TYPE    = 0x3 << 5,
        UNIT        = 0x4 << 5,
        TYPE        = 0x5 << 5,
        DATA        = 0x6 << 5,
        TX          = 0x7 << 5,
        RX          = 0x8 << 5,
        DISP_ADDR   = 0x9 << 5,
        OBJ_ADDR    = 0xA << 5
    };

    // Command opcodes
    enum {
        ALLOC_PROXY = 0x0,
        ALLOC_AGENT = 0x1,
        FREE_BUF    = 0x2
    };

private:
    // FIXME: Without volatile, buf() doesn't work as a while() argument. Why?
    static volatile Reg32 & ctrl(unsigned int o) { return reinterpret_cast<Reg32 *>(CTRL_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & buf(unsigned int o) { return reinterpret_cast<Reg32 *>(BUF_BASE)[o / sizeof(Reg32)]; }
};

__END_SYS

#endif
