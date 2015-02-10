// EPOS Component Manager Abstraction Initialization

#include <component_manager.h>

__BEGIN_SYS

Simple_List<Component_Manager::Recfg_Node> Component_Manager::_nodes;

void Component_Manager::init_ints() {
    Component_Controller::enable_agent_receive_int(&int_handler);
    CPU::int_enable();
}

void Component_Manager::init() {
    // Add all NoC nodes to the _recfg_nodes list. LOCAL_NN is used by the
    // EPOSoC thus not available
    // TODO: The NoC nodes should not be hard coded, maybe use Traits to manage
    // them
    unsigned int addr[2] = { Implementation::Address::LOCAL_NE,
        Implementation::Address::LOCAL_EE };

    for(unsigned int i = 0; i < sizeof(addr)/sizeof(addr[0]); i++) {
        Recfg_Node * node = new (SYSTEM) Recfg_Node(0, 0, addr[i]);
        Simple_List<Recfg_Node>::Element * node_e = new (SYSTEM)
            Simple_List<Recfg_Node>::Element(node);

        _nodes.insert(node_e);
    }

    init_ints();
}

__END_SYS
