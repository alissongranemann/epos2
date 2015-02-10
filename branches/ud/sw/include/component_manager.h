// EPOS Component Manager Abstraction Declarations

#ifndef __component_manager_h
#define __component_manager_h

#include <system/resource.h>
#include <utility/list.h>
//#include <framework/message.h>
#include <framework/id.h>
#include <machine.h>
#include <component_controller.h>

__BEGIN_SYS

class Component_Manager
{
public:
    typedef void (* SW_Dispatcher)(Component_Controller::agent_call_info&);
    // TODO: Fix circular dependency between Component_Manager and Message
    //typedef Message_Common::Method Method;
    typedef int Method;

    // The Recfg_Node is a NoC node which can be used for dynamic partial
    // reconfiguration
    class Recfg_Node
    {
    public:
        Recfg_Node(unsigned int x, unsigned int y, unsigned int local):
            addr(x, y, local) {}

    public:
        Implementation::Address addr;
    };

    // Component_Controller's internal buffers, they are used to implement
    // RTSNoC's handshake
    class Buffer
    {
    public:
        Buffer(Id id): id(id) {
            _node_e = _nodes.remove();

            if(!_node_e) {
                db<Component_Manager>(ERR) << "No Recfg_Node left" << endl;
                Machine::panic();
            }

            node = _node_e->object();
            delete _node_e;

            proxy_id = Component_Controller::alloc_proxy(node->addr, id.type(),
                id.unit());

            if(proxy_id == Component_Controller::CMD_RESULT_ERR) {
                db<Component_Manager>(ERR) << "Couldn't allocate proxy" << endl;
                Machine::panic();
            }
        }

        ~Buffer() {
            _node_e = new (SYSTEM) Simple_List<Recfg_Node>::Element(node);
            _nodes.insert(_node_e);

            Component_Controller::free_buf(proxy_id);
        }

    public:
        Recfg_Node * node;
        unsigned int proxy_id;
        Id id;

    private:
        Simple_List<Recfg_Node>::Element * _node_e;
    };

public:
    static void init();

    static void call(const Buffer & buf, const Method m, unsigned int n_args,
            unsigned int n_ret, unsigned int * data);

private:
    static void int_handler(const unsigned int & interrupt);

    static void init_ints();

private:
    static Simple_List<Recfg_Node> _nodes;
};

__END_SYS

#endif
