// EPOS Component Manager Abstraction Declarations

#ifndef __component_manager_h
#define __component_manager_h

#include <system/resource.h>
#include <system/types_sw.h>
#include <utility/list.h>
#include <machine.h>
#include <cpu.h>
#include <component_controller.h>
#include <pcap.h>
#include <gpio.h>

__BEGIN_SYS

class Component_Manager
{
public:
    typedef void (* SW_Dispatcher)(Component_Controller::agent_call_info&);

    typedef unsigned int Domain;
    enum {
        HARDWARE = 0,
        SOFTWARE = 1
    };

    // The Recfg_Node is a NoC node which can be used for dynamic partial
    // reconfiguration
    class Recfg_Node
    {
    public:
        Recfg_Node(unsigned int x, unsigned int y, unsigned int local,
                unsigned int pin) {
            addr = new (SYSTEM) Implementation::Address(x, y, local);
            decoup_pin = new (SYSTEM) GPIO(pin);

            decoup_pin->output();
        }

        ~Recfg_Node() {
            delete addr;
            delete decoup_pin;
        }

        void couple() { decoup_pin->put(true); }
        void decouple() { decoup_pin->put(false); }

    public:
        Implementation::Address * addr;

    private:
        GPIO * decoup_pin;
    };

    // Component_Controller's internal buffers, they are used to implement
    // RTSNoC's handshake
    class Buffer
    {
    public:
        Buffer(Type_Id type_id, unsigned int inst_id): type_id(type_id),
                inst_id(inst_id) {
            _node_e = _nodes.remove();

            if(!_node_e) {
                db<Component_Manager>(ERR) << "No Recfg_Node left" << endl;
                Machine::panic();
            }

            node = _node_e->object();
            delete _node_e;

            id = Component_Controller::alloc_proxy(*(node->addr), type_id,
                inst_id);

            if(id == Component_Controller::CMD_RESULT_ERR) {
                db<Component_Manager>(ERR) << "Couldn't allocate proxy" << endl;
                Machine::panic();
            }

            node->couple();
        }

        ~Buffer() {
            _node_e = new (SYSTEM) Simple_List<Recfg_Node>::Element(node);
            _nodes.insert(_node_e);

            Component_Controller::free_buf(id);

            node->decouple();
        }

    public:
        Recfg_Node * node;
        unsigned int id;
        Type_Id type_id;
        unsigned int inst_id;

    private:
        Simple_List<Recfg_Node>::Element * _node_e;
    };

    struct Bitstream {
        CPU::Reg32 addr;
        unsigned int n_bytes;
    };

public:
    static void init();

    static void call(Buffer * buf, unsigned int op_id, unsigned int n_args,
            unsigned int n_ret, unsigned int * data);

    static void recfg(Buffer * buf, Type_Id type_id);

public:
    static Implementation::Channel_t dummy_channel;

private:
    static const unsigned int N_RP = 8;

private:
    static void int_handler(unsigned int interrupt);

    static void init_ints();

private:
    static PCAP * _pcap;
    static Simple_List<Recfg_Node> _nodes;
    static Bitstream _bitstreams[N_RP][LAST_TYPE_ID + 1];
};

__END_SYS

#endif
