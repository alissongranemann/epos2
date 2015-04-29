// EPOS Component Manager Abstraction Declarations

#ifndef __component_manager_h
#define __component_manager_h

#include <component_controller.h>

#include "../../unified/rtsnoc.h"

__BEGIN_SYS

class Component_Manager
{
public:
    //typedef void (* SW_Dispatcher)(Component_Controller::agent_call_info&);
    typedef RTSNoC::Address Address;
    typedef Component_Controller::Buffer Buffer;

public:
    static void init();

    template <unsigned int UNIT>
    static void init_buffer();

    static void call(unsigned int type, unsigned int unit, unsigned int m,
            unsigned int n_args, unsigned int n_ret, unsigned long * data);

//private:
    //static void int_handler(const unsigned int & interrupt);

private:
    static Buffer * _nodes[Traits<Component_Manager>::UNITS];
};

__END_SYS

#endif
