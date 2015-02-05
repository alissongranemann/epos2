// EPOS Component Manager Abstraction Initialization

#include <system/resource_table.h>
#include <component_manager.h>
#include <gpio.h>

__BEGIN_SYS

PCAP * Component_Manager::_pcap;
Simple_List<Component_Manager::Recfg_Node> Component_Manager::_nodes;
Component_Manager::Bitstream Component_Manager::_bitstreams[N_RP][LAST_TYPE_ID + 1];

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
    unsigned int decoup_pin[2] = { 55, 56 };

    for(unsigned int i = 0; i < sizeof(addr)/sizeof(addr[0]); i++) {
        Recfg_Node * node = new (SYSTEM) Recfg_Node(0, 0, addr[i], decoup_pin[i]);
        Simple_List<Recfg_Node>::Element * node_e = new (SYSTEM)
            Simple_List<Recfg_Node>::Element(node);

        _nodes.insert(node_e);
    }

    // Populate _bitstreams with the available bitstreams
    // TODO: That's quite ugly :(
    _bitstreams[Implementation::Address::LOCAL_NE][Implementation::ADD_ID].addr = 0x01000000;
    _bitstreams[Implementation::Address::LOCAL_NE][Implementation::ADD_ID].n_bytes = 0x0003004C;
    _bitstreams[Implementation::Address::LOCAL_EE][Implementation::ADD_ID].addr = 0x0103004c;
    _bitstreams[Implementation::Address::LOCAL_EE][Implementation::ADD_ID].n_bytes = 0x0003004C;
    _bitstreams[Implementation::Address::LOCAL_NE][Implementation::MULT_ID].addr = 0x01060098;
    _bitstreams[Implementation::Address::LOCAL_NE][Implementation::MULT_ID].n_bytes = 0x0003004C;
    _bitstreams[Implementation::Address::LOCAL_EE][Implementation::MULT_ID].addr = 0x010900e4;
    _bitstreams[Implementation::Address::LOCAL_EE][Implementation::MULT_ID].n_bytes = 0x0003004C;

    _pcap = new (SYSTEM) PCAP;

    init_ints();
}

__END_SYS
