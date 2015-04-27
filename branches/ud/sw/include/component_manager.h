// EPOS Component Manager Abstraction Declarations

#ifndef __component_manager_h
#define __component_manager_h

//#include <framework/message.h>
#include <framework/id.h>
#include <machine.h>
#include <component_controller.h>

#include "../../unified/framework/rtsnoc.h"

__BEGIN_SYS

class Component_Manager
{
public:
    typedef void (* SW_Dispatcher)(Component_Controller::agent_call_info&);
    // TODO: Fix circular dependency between Component_Manager and Message
    //typedef Message_Common::Method Method;
    typedef int Method;
    typedef RTSNoC::Address Address;

    // Component_Controller's internal buffers, they are used to implement
    // RTSNoC's handshake
    class Buffer
    {
    public:
        Buffer(Id id, Address addr) {
            proxy_id = Component_Controller::alloc_proxy(addr, id.type(),
                id.unit());

            if(proxy_id == Component_Controller::CMD_RESULT_ERR) {
                db<Component_Manager>(ERR) << "Couldn't allocate proxy" << endl;
                Machine::panic();
            }
        }

        ~Buffer() { Component_Controller::free_buf(proxy_id); }

    private:
        unsigned int proxy_id;
    };

public:
    static void init();

    template <unsigned int UNIT>
    static void init_buffer();

    static void call(Id id, const Method m, unsigned int n_args,
        unsigned int n_ret, unsigned long * data);

private:
    static void int_handler(const unsigned int & interrupt);

private:
    static Buffer * _nodes[Traits<Component_Manager>::UNITS];
};

__END_SYS

#endif
